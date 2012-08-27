// XMLFileLoader.cpp
// Implements file reader for an XML format
#include <iostream>
#include <string>
#include <sys/stat.h>
#include <set>
#include <string>

#include "QueryEngine.h"
#include "XMLFileLoader.h"
#include "XMLQueryEngine.h"

#include "Env.h"
#include "Timer.h"
#include "RecipeLibrary.h"
#include "Model.h"

#include "Env.h"
#include "CycException.h"
#include "Logger.h"

using namespace std;

// static members
std::string XMLFileLoader::main_schema_ = Env::getInstallPath() + "/share/cyclus.rng";

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
XMLFileLoader::XMLFileLoader(std::string load_filename) {

  // double check that the file exists
  if("" == load_filename) {
    throw CycIOException("No input filename was given");
  } else { 
    FILE* file = fopen(load_filename.c_str(),"r");
    if (NULL == file) { 
      throw CycIOException("The file '" + load_filename
           + "' cannot be loaded because it has not been found.");
    }
    fclose(file);
  }

  filename_ = load_filename;
  initialize_module_paths();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void XMLFileLoader::initialize_module_paths() {
  module_paths_["Market"] = "/*/market";
  module_paths_["Converter"] = "/*/converter";
  module_paths_["Region"] = "/simulation/region";
  module_paths_["Inst"] = "/simulation/region/institution";
  module_paths_["Facility"] = "/*/facility";
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void XMLFileLoader::validate_file(std::string schema_file) {

  xmlRelaxNGParserCtxtPtr ctxt = xmlRelaxNGNewParserCtxt(schema_file.c_str());
  if (NULL == ctxt)
    throw CycParseException("Failed to generate parser from schema: " + schema_file);

  xmlRelaxNGPtr schema = xmlRelaxNGParse(ctxt);

  xmlRelaxNGValidCtxtPtr vctxt = xmlRelaxNGNewValidCtxt(schema);

  doc_ = xmlReadFile(filename_.c_str(), NULL,0);
  if (NULL == doc_) {
    throw CycParseException("Failed to parse: " + filename_);
  }

  if (xmlRelaxNGValidateDoc(vctxt,doc_))
    throw CycParseException("Invalid XML file; file: "    
      + filename_ 
      + " does not validate against schema " 
      + schema_file);

  /// free up some data ???

}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void XMLFileLoader::load_recipes() {
  XMLQueryEngine xqe(doc_);

  string query = "/*/recipe";
  int numRecipes = xqe.numElementsMatchingQuery(query);
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
  XMLQueryEngine xqe(doc_);
  
  int numModels = xqe.numElementsMatchingQuery(query_path);
  for (int i=0; i<numModels; i++) {
    QueryEngine* qe = xqe.queryElement(query_path,i);
    Model::initializeSimulationEntity(type,qe);
  }
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void XMLFileLoader::load_control_parameters() {
  XMLQueryEngine xqe(doc_);

  string query = "/*/control";
  QueryEngine* qe = xqe.queryElement(query);
  TI->load_simulation(qe);
}
  
