// XMLFileLoader.cpp
// Implements file reader for an XML format
#include "XMLFileLoader.h"

#include <fstream>

#include <boost/filesystem.hpp>

#include "blob.h"
#include "Env.h"
#include "Error.h"
#include "EventManager.h"
#include "Model.h"
#include "RecipeLibrary.h"
#include "Timer.h"
#include "XMLQueryEngine.h"

namespace cyclus {
namespace fs = boost::filesystem;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
XMLFileLoader::XMLFileLoader(const std::string load_filename) {
  file_ = load_filename;
  initialize_module_paths();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void XMLFileLoader::init(bool use_main_schema)  {
  std::stringstream input("");
  loadStringstreamFromFile(input, file_);
  parser_ = boost::shared_ptr<XMLParser>(new XMLParser());
  parser_->init(input);
  if (use_main_schema) {
    std::stringstream ss(buildSchema());
    parser_->validate(ss);
  }

  EM->newEvent("InputFiles")
  ->addVal("Data", cyclus::Blob(input.str()))
  ->record();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::string XMLFileLoader::buildSchema() {
  std::stringstream schema("");
  loadStringstreamFromFile(schema, pathToMainSchema());
  std::string master = schema.str();

  std::set<std::string> types = Model::dynamic_module_types();

  std::stringstream includes;
  std::set<std::string>::iterator type;
  for (type = types.begin(); type != types.end(); ++type) {
    // find modules
    std::stringstream refs;
    refs << std::endl;
    fs::path models_path = Env::getInstallPath() + "/lib/Models/" + *type;
    fs::recursive_directory_iterator end;
    try {
      for (fs::recursive_directory_iterator it(models_path); it != end; ++it) {
        fs::path p = it->path();
        // build refs and includes
        if (p.extension() == ".rng") {
          refs << "<ref name='" << p.stem().string() << "'/>" << std::endl;
          includes << "<include href='" << p.string() << "'/>" << std::endl;
        }
      }
    } catch (...) { }

    // replace refs
    std::string searchStr = std::string("@") + *type + std::string("_REFS@");
    size_t pos = master.find(searchStr);
    if (pos != std::string::npos) {
      master.replace(pos, searchStr.size(), refs.str());
    }
  }

  // replace includes
  std::string searchStr = "@RNG_INCLUDES@";
  size_t pos = master.find(searchStr);
  if (pos != std::string::npos) {
    master.replace(pos, searchStr.size(), includes.str());
  }

  return master;
}

// - - - - - - - - - - - - - - - -   - - - - - - - - - - - - - - - - - -
void XMLFileLoader::loadStringstreamFromFile(std::stringstream& stream,
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
std::string XMLFileLoader::pathToMainSchema() {
  return Env::getInstallPath() + "/share/cyclus.rng.in";
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void XMLFileLoader::applySchema(const std::stringstream& schema) {
  parser_->validate(schema);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void XMLFileLoader::initialize_module_paths() {
  module_paths_["Market"] = "/*/market";
  module_paths_["Converter"] = "/*/converter";
  module_paths_["Region"] = "/*/region";
  module_paths_["Inst"] = "/*/region/institution";
  module_paths_["Facility"] = "/*/facility";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void XMLFileLoader::load_recipes() {
  XMLQueryEngine xqe(*parser_);

  std::string query = "/*/recipe";
  int numRecipes = xqe.nElementsMatchingQuery(query);
  for (int i = 0; i < numRecipes; i++) {
    QueryEngine* qe = xqe.queryElement(query, i);
    RecipeLibrary::load_recipe(qe);
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void XMLFileLoader::load_dynamic_modules(std::set<std::string>& module_types) {
  std::set<std::string>::iterator it;
  for (it = module_types.begin(); it != module_types.end(); it++) {
    load_modules_of_type(*it, module_paths_[*it]);
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void XMLFileLoader::load_modules_of_type(std::string type,
                                         std::string query_path) {
  XMLQueryEngine xqe(*parser_);

  int numModels = xqe.nElementsMatchingQuery(query_path);
  for (int i = 0; i < numModels; i++) {
    QueryEngine* qe = xqe.queryElement(query_path, i);
    Model::initializeSimulationEntity(type, qe);
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void XMLFileLoader::load_control_parameters() {
  XMLQueryEngine xqe(*parser_);

  std::string query = "/*/control";
  QueryEngine* qe = xqe.queryElement(query);
  TI->load_simulation(qe);
}
} // namespace cyclus
