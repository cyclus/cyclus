// xml_file_loader.cc
// Implements file reader for an XML format
#include <algorithm>
#include <fstream>
#include <set>
#include <streambuf>

#include <boost/filesystem.hpp>

#include "blob.h"
#include "context.h"
#include "cyc_std.h"
#include "env.h"
#include "error.h"
#include "greedy_preconditioner.h"
#include "greedy_solver.h"
#include "logger.h"
#include "model.h"
#include "recorder.h"
#include "timer.h"
#include "xml_query_engine.h"

#include "xml_file_loader.h"

namespace cyclus {

namespace fs = boost::filesystem;

// - - - - - - - - - - - - - - - -   - - - - - - - - - - - - - - - - - -
void LoadStringstreamFromFile(std::stringstream& stream,
                              std::string file) {
  std::ifstream file_stream(file.c_str());
  if (!file_stream) {
    throw IOError("The file '" + file + "' could not be loaded.");
  }

  stream << file_stream.rdbuf();
  file_stream.close();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::string BuildMasterSchema(std::string schema_path) {
  Timer ti;
  Recorder rec;
  Context ctx(&ti, &rec);

  std::stringstream schema("");
  LoadStringstreamFromFile(schema, schema_path);
  std::string master = schema.str();

  std::vector<std::string> names = Env::ListModules();
  std::map<std::string, std::string> subschemas;
  for (int i = 0; i < names.size(); ++i) {
    DynamicModule dyn(names[i]);
    Model* m = dyn.ConstructInstance(&ctx);
    subschemas[m->model_type()] += "<element name=\"" + names[i] + "\">\n";
    subschemas[m->model_type()] += m->schema() + "\n";
    subschemas[m->model_type()] += "</element>\n";
    ctx.DelModel(m);
    dyn.CloseLibrary();
  }

  // replace refs in master rng template file
  std::map<std::string, std::string>::iterator it;
  for (it = subschemas.begin(); it != subschemas.end(); ++it) {
    std::string search_str = std::string("@") + it->first + std::string("_REFS@");
    size_t pos = master.find(search_str);
    if (pos != std::string::npos) {
      master.replace(pos, search_str.size(), it->second);
    }
  }

  return master;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Composition::Ptr ReadRecipe(QueryEngine* qe) {
  bool atom_basis;
  std::string basis_str = qe->GetElementContent("basis");
  if (basis_str == "atom") {
    atom_basis = true;
  } else if (basis_str == "mass") {
    atom_basis = false;
  } else {
    throw IOError(basis_str + " basis is not 'mass' or 'atom'.");
  }

  double value;
  int key;
  std::string query = "nuclide";
  int nnucs = qe->NElementsMatchingQuery(query);
  CompMap v;
  for (int i = 0; i < nnucs; i++) {
    QueryEngine* nuclide = qe->QueryElement(query, i);
    key = strtol(nuclide->GetElementContent("id").c_str(), NULL, 10);
    value = strtod(nuclide->GetElementContent("comp").c_str(), NULL);
    v[key] = value;
    CLOG(LEV_DEBUG3) << "  Nuclide: " << key << " Value: " << v[key];
  }

  if (atom_basis) {
    return Composition::CreateFromAtom(v);
  } else {
    return Composition::CreateFromMass(v);
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
XMLFileLoader::XMLFileLoader(Context* ctx,
                             std::string schema_path,
                             const std::string load_filename) : ctx_(ctx) {
  schema_path_ = schema_path;
  file_ = load_filename;
  std::stringstream input;
  LoadStringstreamFromFile(input, file_);
  parser_ = boost::shared_ptr<XMLParser>(new XMLParser());
  parser_->Init(input);

  ctx_->NewDatum("InputFiles")
  ->AddVal("Data", Blob(input.str()))
  ->Record();

  schema_paths_["Region"] = "/*/region";
  schema_paths_["Inst"] = "/*/region/institution";
  schema_paths_["Facility"] = "/*/facility";
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
XMLFileLoader::~XMLFileLoader() {
  std::map<std::string, DynamicModule*>::iterator it;
  for (it = modules_.begin(); it != modules_.end(); it++) {
    it->second->CloseLibrary();
    delete it->second;
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void XMLFileLoader::ApplySchema(const std::stringstream& schema) {
  parser_->Validate(schema);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::string XMLFileLoader::master_schema() {
  return BuildMasterSchema(schema_path_);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void XMLFileLoader::LoadSim() {
  try {
    LoadDynamicModules();
    std::stringstream ss(master_schema());
    parser_->Validate(ss);
    LoadSolver();
    LoadControlParams();
    LoadRecipes();
    LoadInitialAgents();
  } catch (std::exception& e) {
    std::string msg = "Error reading xml file: ";
    msg += e.what();
    throw cyclus::Error(msg);
  }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void XMLFileLoader::LoadSolver() {
  XMLQueryEngine xqe(*parser_);
  std::string query = "/*/commodity";

  std::map<std::string, double> commodity_order;
  std::string name;
  double order;
  int num_commods = xqe.NElementsMatchingQuery(query);
  for (int i = 0; i < num_commods; i++) {
    QueryEngine* qe = xqe.QueryElement(query, i);
    name = qe->GetElementContent("name");
    order = GetOptionalQuery<double>(qe, "solution_order", -1);
    commodity_order[name] = order;
  }

  ProcessCommodities(&commodity_order);

  // solver will delete conditioner
  GreedyPreconditioner* conditioner = new GreedyPreconditioner(
    commodity_order,
    GreedyPreconditioner::REVERSE);

  // context will delete solver
  bool exclusive_orders = false;
  GreedySolver* solver = new GreedySolver(exclusive_orders, conditioner);

  ctx_->solver(solver);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void XMLFileLoader::ProcessCommodities(
  std::map<std::string, double>* commodity_order) {
  double max = std::max_element(
                 commodity_order->begin(),
                 commodity_order->end(),
                 SecondLT< std::pair<std::string, double> >())->second;
  if (max < 1) {
    max = 0;  // in case no orders are specified
  }

  std::map<std::string, double>::iterator it;
  for (it = commodity_order->begin();
       it != commodity_order->end();
       ++it) {
    if (it->second < 1) {
      it->second = max + 1;
    }
    CLOG(LEV_INFO1) << "Commodity ordering for " << it->first
                    << " is " << it->second;
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void XMLFileLoader::LoadRecipes() {
  XMLQueryEngine xqe(*parser_);

  std::string query = "/*/recipe";
  int num_recipes = xqe.NElementsMatchingQuery(query);
  for (int i = 0; i < num_recipes; i++) {
    QueryEngine* qe = xqe.QueryElement(query, i);
    std::string name = qe->GetElementContent("name");
    CLOG(LEV_DEBUG3) << "loading recipe: " << name;
    ctx_->AddRecipe(name, ReadRecipe(qe));
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void XMLFileLoader::LoadInitialAgents() {
  std::set<std::string> module_types;
  module_types.insert("Region");
  module_types.insert("Inst");
  module_types.insert("Facility");
  std::set<std::string>::iterator it;
  for (it = module_types.begin(); it != module_types.end(); it++) {
    XMLQueryEngine xqe(*parser_);
    int num_models = xqe.NElementsMatchingQuery(schema_paths_[*it]);
    for (int i = 0; i < num_models; i++) {
      QueryEngine* qe = xqe.QueryElement(schema_paths_[*it], i);
      QueryEngine* module_data = qe->QueryElement("model");
      std::string module_name = module_data->GetElementName();

      Model* model = modules_[module_name]->ConstructInstance(ctx_);
      model->SetModelImpl(module_name);
      model->InitFrom(qe);

      CLOG(LEV_DEBUG3) << "Module '" << model->name()
                       << "' has had its module members initialized:";
      CLOG(LEV_DEBUG3) << " * Type: " << model->model_type();
      CLOG(LEV_DEBUG3) << " * Implementation: " << model->ModelImpl() ;
      CLOG(LEV_DEBUG3) << " * ID: " << model->id();

      // register module
      ctx_->AddPrototype(model->name(), model);
      if (*it == "Region") {
        model->Deploy();
      }
    }
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void XMLFileLoader::LoadDynamicModules() {
  std::vector<std::string> names = Env::ListModules();
  for (int i = 0; i < names.size(); ++i) {
    DynamicModule* module = new DynamicModule(names[i]);
    modules_[names[i]] = module;
    CLOG(LEV_DEBUG1) << "Module '" << names[i]
                     << "' has been loaded.";
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void XMLFileLoader::LoadControlParams() {
  XMLQueryEngine xqe(*parser_);
  std::string query = "/*/control";
  QueryEngine* qe = xqe.QueryElement(query);

  std::string handle;
  if (qe->NElementsMatchingQuery("simhandle") > 0) {
    handle = qe->GetElementContent("simhandle");
  }

  // get duration
  std::string dur_str = qe->GetElementContent("duration");
  int dur = strtol(dur_str.c_str(), NULL, 10);
  // get start month
  std::string m0_str = qe->GetElementContent("startmonth");
  int m0 = strtol(m0_str.c_str(), NULL, 10);
  // get start year
  std::string y0_str = qe->GetElementContent("startyear");
  int y0 = strtol(y0_str.c_str(), NULL, 10);
  // get simulation start
  std::string sim0_str = qe->GetElementContent("simstart");
  int sim0 = strtol(sim0_str.c_str(), NULL, 10);
  // get decay interval
  std::string decay_str = qe->GetElementContent("decay");
  int dec = strtol(decay_str.c_str(), NULL, 10);

  ctx_->InitTime(sim0, dur, dec, m0, y0, handle);
}

} // namespace cyclus


