// xml_file_loader.cc
// Implements file reader for an XML format
#include "xml_file_loader.h"

#include <fstream>
#include <streambuf>

#include <boost/filesystem.hpp>

#include "blob.h"
#include "context.h"
#include "env.h"
#include "error.h"
#include "logger.h"
#include "model.h"
#include "xml_query_engine.h"

namespace cyclus {
namespace fs = boost::filesystem;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
XMLFileLoader::XMLFileLoader(Context* ctx,
                             const std::string load_filename,
                             bool use_main_schema) : ctx_(ctx) {
  file_ = load_filename;
  initialize_module_paths();

  std::stringstream input("");
  LoadStringstreamFromFile(input, file_);
  parser_ = boost::shared_ptr<XMLParser>(new XMLParser());
  parser_->Init(input);
  if (use_main_schema) {
    std::stringstream ss(BuildSchema());
    parser_->Validate(ss);
  }

  ctx_->NewEvent("InputFiles")
  ->AddVal("Data", cyclus::Blob(input.str()))
  ->Record();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::string XMLFileLoader::BuildSchema() {
  std::stringstream schema("");
  LoadStringstreamFromFile(schema, PathToMainSchema());
  std::string master = schema.str();

  std::set<std::string> types = Model::dynamic_module_types();

  std::stringstream includes;
  std::set<std::string>::iterator type;
  for (type = types.begin(); type != types.end(); ++type) {
    // find modules
    std::stringstream refs;
    refs << std::endl;
    fs::path models_path = Env::GetInstallPath() + "/lib/Models/" + *type;
    fs::recursive_directory_iterator end;
    try {
      for (fs::recursive_directory_iterator it(models_path); it != end; ++it) {
        fs::path p = it->path();
        // build refs and includes
        if (p.extension() == ".rng") {
          includes << "<include href='" << p.string() << "'/>" << std::endl;

          std::ifstream in(p.string().c_str(), std::ios::in | std::ios::binary);
          std::string rng_data((std::istreambuf_iterator<char>(in)),
                               std::istreambuf_iterator<char>());
          std::string find_str("<define name=\"");
          size_t start = rng_data.find(find_str) + find_str.size();
          size_t end = rng_data.find("\"", start + 1);
          std::string ref = rng_data.substr(start, end - start);
          refs << "<ref name='" << ref << "'/>" << std::endl;
        }
      }
    } catch (...) { }

    // replace refs
    std::string search_str = std::string("@") + *type + std::string("_REFS@");
    size_t pos = master.find(search_str);
    if (pos != std::string::npos) {
      master.replace(pos, search_str.size(), refs.str());
    }
  }

  // replace includes
  std::string search_str = "@RNG_INCLUDES@";
  size_t pos = master.find(search_str);
  if (pos != std::string::npos) {
    master.replace(pos, search_str.size(), includes.str());
  }

  return master;
}

// - - - - - - - - - - - - - - - -   - - - - - - - - - - - - - - - - - -
void XMLFileLoader::LoadStringstreamFromFile(std::stringstream& stream,
                                             std::string file) {

  CLOG(LEV_DEBUG4) << "loading the file: " << file;

  std::ifstream file_stream(file.c_str());

  if (file_stream) {
    stream << file_stream.rdbuf();
    file_stream.close();
  } else {
    throw IOError("The file '" + file
                  + "' could not be loaded.");
  }

  CLOG(LEV_DEBUG5) << "file loaded as a string: " << stream.str();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::string XMLFileLoader::PathToMainSchema() {
  return Env::GetInstallPath() + "/share/cyclus.rng.in";
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void XMLFileLoader::ApplySchema(const std::stringstream& schema) {
  parser_->Validate(schema);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void XMLFileLoader::initialize_module_paths() {
  module_paths_["Market"] = "/*/market";
  module_paths_["Region"] = "/*/region";
  module_paths_["Inst"] = "/*/region/institution";
  module_paths_["Facility"] = "/*/facility";
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void XMLFileLoader::LoadAll() {
  LoadControlParams();
  LoadRecipes();
  LoadDynamicModules();
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
void XMLFileLoader::LoadDynamicModules() {
  std::set<std::string> module_types = Model::dynamic_module_types();
  std::set<std::string>::iterator it;
  for (it = module_types.begin(); it != module_types.end(); it++) {
    LoadModulesOfType(*it, module_paths_[*it]);
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void XMLFileLoader::LoadModulesOfType(std::string type,
                                      std::string query_path) {
  XMLQueryEngine xqe(*parser_);

  int num_models = xqe.NElementsMatchingQuery(query_path);
  for (int i = 0; i < num_models; i++) {
    QueryEngine* qe = xqe.QueryElement(query_path, i);
    Model::InitializeSimulationEntity(ctx_, type, qe);
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
