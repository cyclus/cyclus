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
#include "xml_query_engine.h"

#include "xml_file_loader.h"

namespace cyclus {

namespace fs = boost::filesystem;

void LoadStringstreamFromFile(std::stringstream& stream,
                              std::string file) {
  std::ifstream file_stream(file.c_str());
  if (!file_stream) {
    throw IOError("The file '" + file + "' could not be loaded.");
  }

  stream << file_stream.rdbuf();
  file_stream.close();
}

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
    Model* m = DynamicModule::Make(&ctx, names[i]);
    subschemas[m->kind()] += "<element name=\"" + names[i] + "\">\n";
    subschemas[m->kind()] += m->schema() + "\n";
    subschemas[m->kind()] += "</element>\n";
    ctx.DelModel(m);
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

XMLFileLoader::XMLFileLoader(FullBackend* b,
                             std::string schema_path,
                             const std::string load_filename) : fb_(b) {
  rec_.RegisterBackend(b);
  ctx_ = new Context(&ti_, &rec_);

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

XMLFileLoader::~XMLFileLoader() {
  rec_.Close();
  delete ctx_;
}

void XMLFileLoader::ApplySchema(const std::stringstream& schema) {
  parser_->Validate(schema);
}

std::string XMLFileLoader::master_schema() {
  return BuildMasterSchema(schema_path_);
}

void XMLFileLoader::LoadSim() {
  std::stringstream ss(master_schema());
  parser_->Validate(ss);
  LoadControlParams(); // must be first
  LoadSolver();
  LoadRecipes();
  LoadInitialAgents(); // must be last
  ctx_->Snapshot();
};

void XMLFileLoader::LoadSolver() {
  XMLQueryEngine xqe(*parser_);
  std::string query = "/*/commodity";

  std::map<std::string, double> commod_order;
  std::string name;
  double order;
  int num_commods = xqe.NElementsMatchingQuery(query);
  for (int i = 0; i < num_commods; i++) {
    QueryEngine* qe = xqe.QueryElement(query, i);
    name = qe->GetElementContent("name");
    order = GetOptionalQuery<double>(qe, "solution_order", -1);
    commod_order[name] = order;
  }

  ProcessCommodities(&commod_order);
  std::map<std::string, double>::iterator it;
  for (it = commod_order.begin(); it != commod_order.end(); ++it) {
    ctx_->NewDatum("CommodPriority")
      ->AddVal("Commodity", it->first)
      ->AddVal("SolutionOrder", it->second)
      ->Record();
  }
}

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

void XMLFileLoader::LoadRecipes() {
  XMLQueryEngine xqe(*parser_);

  std::string query = "/*/recipe";
  int num_recipes = xqe.NElementsMatchingQuery(query);
  for (int i = 0; i < num_recipes; i++) {
    QueryEngine* qe = xqe.QueryElement(query, i);
    std::string name = qe->GetElementContent("name");
    CLOG(LEV_DEBUG3) << "loading recipe: " << name;
    Composition::Ptr comp = ReadRecipe(qe);
    comp->Record(ctx_);
    ctx_->AddRecipe(name, comp);
  }
}

void XMLFileLoader::LoadInitialAgents() {
  DbInit di;
  std::set<std::string> module_types;
  module_types.insert("Region");
  module_types.insert("Inst");
  module_types.insert("Facility");
  std::set<std::string>::iterator it;
  XMLQueryEngine xqe(*parser_);

  // create prototypes
  std::string prototype; // defined here for force-create AgentExit tbl
  for (it = module_types.begin(); it != module_types.end(); it++) {
    int num_models = xqe.NElementsMatchingQuery(schema_paths_[*it]);
    for (int i = 0; i < num_models; i++) {
      QueryEngine* qe = xqe.QueryElement(schema_paths_[*it], i);
      QueryEngine* module_data = qe->QueryElement("model");
      std::string module_name = module_data->GetElementName();
      prototype = qe->GetElementContent("name");

      Model* model = DynamicModule::Make(ctx_, module_name);
      model->set_model_impl(module_name);
      model->InfileToDb(qe, di);
      rec_.Flush();
      std::vector<Cond> conds;
      conds.push_back(Cond("SimId", "==", rec_.sim_id()));
      conds.push_back(Cond("AgentId", "==", model->id()));
      CondInjector ci(fb_, conds);
      PrefixInjector pi(&ci, "AgentState_");
      model->InitFrom(&pi);
      ctx_->AddPrototype(prototype, model);
    }
  }

  // build initial agent instances
  int nregions = xqe.NElementsMatchingQuery(schema_paths_["Region"]);
  for (int i = 0; i < nregions; ++i) {
    QueryEngine* qe = xqe.QueryElement(schema_paths_["Region"], i);
    std::string region_proto = qe->GetElementContent("name");
    Model* reg = BuildAgent(region_proto, NULL);

    int ninsts = qe->NElementsMatchingQuery("institution");
    for (int j = 0; j < ninsts; ++j) {
      QueryEngine* qe2 = qe->QueryElement("institution", j);
      std::string inst_proto = qe2->GetElementContent("name");
      Model* inst = BuildAgent(inst_proto, reg);

      int nfac = qe2->NElementsMatchingQuery("initialfacilitylist/entry");
      for (int k = 0; k < nfac; ++k) {
        QueryEngine* qe3 = qe2->QueryElement("initialfacilitylist/entry", k);
        std::string fac_proto = qe3->GetElementContent("prototype");

        int number = atoi(qe3->GetElementContent("number").c_str());
        for (int z = 0; z < number; ++z) {
          Model* fac = BuildAgent(fac_proto, inst);
          LoadInventory(fac, qe3);
        }
      }
    }
  }
}

void XMLFileLoader::LoadInventory(Model* m, QueryEngine* qe) {
  Inventories invs;
  int ninvs = qe->NElementsMatchingQuery("inventories/inv");
  for (int i = 0; i < ninvs; ++i) {
    QueryEngine* qe2 = qe->QueryElement("inventories/inv", i);
    std::string name = qe2->GetElementContent("name");
    int nmats = qe2->NElementsMatchingQuery("materials");
    for (int j = 0; j < nmats; ++j) {
      QueryEngine* qe3 = qe2->QueryElement("materials", j);
      std::string recipe = qe3->GetElementContent("recipe");
      double qty = GetOptionalQuery<double>(qe3, "quantity", 0);
      Material::Ptr mat = Material::Create(m, qty, ctx_->GetRecipe(recipe));
      invs[name].push_back(mat);
    }
  }
  m->InitInv(invs);
}

Model* XMLFileLoader::BuildAgent(std::string proto, Model* parent) {
  Model* m = ctx_->CreateModel<Model>(proto);
  m->Build(parent);
  if (parent != NULL) {
    parent->BuildNotify(m);
  }
  return m;
}

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
  // get decay interval
  std::string decay_str = qe->GetElementContent("decay");
  int dec = strtol(decay_str.c_str(), NULL, 10);

  ctx_->InitSim(SimInfo(dur, y0, m0, dec, handle));
}

} // namespace cyclus

