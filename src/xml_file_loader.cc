// xml_file_loader.cc
// Implements file reader for an XML format
#include "xml_file_loader.h"

#include <fstream>
#include <set>
#include <streambuf>

#include <boost/filesystem.hpp>

#include "blob.h"
#include "context.h"
#include "env.h"
#include "error.h"
#include "event_manager.h"
#include "logger.h"
#include "model.h"
#include "timer.h"
#include "xml_query_engine.h"

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
std::string BuildMasterSchema() {
  Timer ti;
  EventManager em;
  Context ctx(&ti, &em);

  std::stringstream schema("");
  std::string schema_path = Env::GetInstallPath() + "/share/cyclus.rng.in";
  LoadStringstreamFromFile(schema, schema_path);
  std::string master = schema.str();

  std::vector<std::string> names = Env::ListModules();
  std::map<std::string, std::string> subschemas;
  for (int i = 0; i < names.size(); ++i) {
    DynamicModule dyn(names[i]);
    Model* m = dyn.ConstructInstance(&ctx);
    subschemas[m->ModelType()] += "<element name=\"" + names[i] + "\">\n";
    subschemas[m->ModelType()] += m->schema() + "\n";
    subschemas[m->ModelType()] += "</element>\n";
    delete m;
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
XMLFileLoader::XMLFileLoader(Context* ctx,
                             const std::string load_filename) : ctx_(ctx) {
  file_ = load_filename;
  std::stringstream input;
  LoadStringstreamFromFile(input, file_);
  parser_ = boost::shared_ptr<XMLParser>(new XMLParser());
  parser_->Init(input);

  ctx_->NewEvent("InputFiles")
  ->AddVal("Data", Blob(input.str()))
  ->Record();

  schema_paths_["Market"] = "/*/market";
  schema_paths_["Converter"] = "/*/converter";
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
void XMLFileLoader::LoadSim(bool use_main_schema) {
  LoadDynamicModules();

  if (use_main_schema) {
    std::stringstream ss(BuildMasterSchema());
    parser_->Validate(ss);
  }

  LoadControlParams();
  LoadRecipes();
  LoadInitialAgents();
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void XMLFileLoader::LoadRecipes() {
  XMLQueryEngine xqe(*parser_);

  std::string query = "/*/recipe";
  int num_recipes = xqe.NElementsMatchingQuery(query);
  for (int i = 0; i < num_recipes; i++) {
    QueryEngine* qe = xqe.QueryElement(query, i);
    LoadRecipe(qe);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void XMLFileLoader::LoadRecipe(QueryEngine* qe) {
  bool atom_basis;
  std::string basis_str = qe->GetElementContent("basis");
  if (basis_str == "atom") {
    atom_basis = true;
  } else if (basis_str == "mass") {
    atom_basis = false;
  } else {
    throw IOError(basis_str + " basis is not 'mass' or 'atom'.");
  }

  std::string name = qe->GetElementContent("name");
  CLOG(LEV_DEBUG3) << "loading recipe: " << name
                   << " with basis: " << basis_str;

  double value;
  int key;
  std::string query = "isotope";
  int nisos = qe->NElementsMatchingQuery(query);
  CompMap v;
  for (int i = 0; i < nisos; i++) {
    QueryEngine* isotope = qe->QueryElement(query, i);
    key = strtol(isotope->GetElementContent("id").c_str(), NULL, 10);
    value = strtod(isotope->GetElementContent("comp").c_str(), NULL);
    v[key] = value;
    CLOG(LEV_DEBUG3) << "  Isotope: " << key << " Value: " << v[key];
  }

  if (atom_basis) {
    ctx_->AddRecipe(name, Composition::CreateFromAtom(v));
  } else {
    ctx_->AddRecipe(name, Composition::CreateFromMass(v));
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void XMLFileLoader::LoadInitialAgents() {
  std::set<std::string> module_types = Model::dynamic_module_types();
  std::set<std::string>::iterator it;
  for (it = module_types.begin(); it != module_types.end(); it++) {
    XMLQueryEngine xqe(*parser_);
    int num_models = xqe.NElementsMatchingQuery(schema_paths_[*it]);
    for (int i = 0; i < num_models; i++) {
      QueryEngine* qe = xqe.QueryElement(schema_paths_[*it], i);
      QueryEngine* module_data = qe->QueryElement("model");
      std::string module_name = module_data->GetElementName();

      Model* model = modules_[module_name]->ConstructInstance(ctx_);
      model->InitCoreMembers(qe);
      model->SetModelImpl(module_name);
      model->InitModuleMembers(module_data->QueryElement(module_name));

      CLOG(LEV_DEBUG3) << "Module '" << model->name()
                       << "' has had its module members initialized:";
      CLOG(LEV_DEBUG3) << " * Type: " << model->ModelType();
      CLOG(LEV_DEBUG3) << " * Implementation: " << model->ModelImpl() ;
      CLOG(LEV_DEBUG3) << " * ID: " << model->id();

      // register module
      if (*it == "Facility") {
        ctx_->AddPrototype(model->name(), model);
      } else if (*it == "Market" || *it == "Region") {
        model->Deploy(model);
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
