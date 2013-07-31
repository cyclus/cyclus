// XMLFileLoader.cpp
// Implements file reader for an XML format
#include "XMLFileLoader.h"

#include <fstream>

#include "XMLQueryEngine.h"

#include "Env.h"
#include "Timer.h"
#include "RecipeLibrary.h"
#include "Model.h"
#include "CycException.h"

namespace cyclus {

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
    std::stringstream schema("");
    loadStringstreamFromFile(schema, pathToMainSchema());
    parser_->validate(schema);
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void XMLFileLoader::loadStringstreamFromFile(std::stringstream &stream,
                                             std::string file) {

  CLOG(LEV_DEBUG4) << "loading the file: " << file;

  std::ifstream file_stream(file.c_str());

  if (file_stream) {
    stream << file_stream.rdbuf();
    file_stream.close();
  } else {
    throw CycIOException("The file '" + file
                         + "' could not be loaded.");
  }

  CLOG(LEV_DEBUG5) << "file loaded as a string: " << stream.str();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::string XMLFileLoader::pathToMainSchema() {
  return Env::getInstallPath() + "/share/cyclus.rng";
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void XMLFileLoader::applySchema(const std::stringstream &schema) {
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
  for (int i=0; i<numRecipes; i++) {
    QueryEngine* qe = xqe.queryElement(query,i);
    RecipeLibrary::load_recipe(qe);
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void XMLFileLoader::load_dynamic_modules(std::set<std::string>& module_types) {
  std::set<std::string>::iterator it;
  for (it = module_types.begin(); it != module_types.end(); it++) {
    load_modules_of_type(*it,module_paths_[*it]);
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void XMLFileLoader::load_modules_of_type(std::string type, 
                                         std::string query_path) {  
  XMLQueryEngine xqe(*parser_);
  
  int numModels = xqe.nElementsMatchingQuery(query_path);
  for (int i=0; i<numModels; i++) {
    QueryEngine* qe = xqe.queryElement(query_path,i);
    Model::initializeSimulationEntity(type,qe);
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
