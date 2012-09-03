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

using namespace std;
using namespace boost;

// static members
std::string XMLFileLoader::main_schema_ = Env::getInstallPath() + "/share/cyclus.rng";

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
XMLFileLoader::XMLFileLoader(std::string load_filename, 
                             bool use_main_schema) {
  initialize_module_paths();

  stringstream input("");
  loadStringstreamFromFile(input,load_filename);
  if (use_main_schema) {
    stringstream schema("");
    loadStringstreamFromFile(schema,main_schema_);
    parser_ = shared_ptr<XMLParser>(new XMLParser(input,schema));
  } else {
    parser_ = shared_ptr<XMLParser>(new XMLParser(input));
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void XMLFileLoader::loadStringstreamFromFile(std::stringstream &stream,
                                             std::string file) {
  ifstream file_stream(file.c_str());

  if (file_stream) {
    stream << file_stream.rdbuf();
    file_stream.close();
  } else {
    throw CycIOException("The file '" + file
                         + "' could not be loaded.");
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void XMLFileLoader::applySchema(std::stringstream &schema) {
  parser_->validateFileAgaisntSchema(schema);
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

  string query = "/*/recipe";
  int numRecipes = xqe.nElementsMatchingQuery(query);
  for (int i=0; i<numRecipes; i++) {
    QueryEngine* qe = xqe.queryElement(query,i);
    RecipeLibrary::load_recipe(qe);
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void XMLFileLoader::load_dynamic_modules(std::set<std::string>& module_types) {
  set<string>::iterator it;
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

  string query = "/*/control";
  QueryEngine* qe = xqe.queryElement(query);
  TI->load_simulation(qe);
}
  
