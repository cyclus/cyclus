// XMLFileLoader.cpp
// Implements file reader for an XML format
#include <iostream>
#include <string>
#include <sys/stat.h>

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
std::string XMLFileLoader::recipe_book_schema_ = "cyclus.rng";
std::string XMLFileLoader::facility_catalog_schema_ = "cyclus.rng";

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
XMLFileLoader::XMLFileLoader(std::string load_filename) {
  // double check that the file exists
  if("" == load_filename) {
    throw CycIOException("No input filename was given");
  } else { 
    FILE* file = fopen(load_filename.c_str(),"r");
    if (NULL == file) { 
      throw CycIOException("The file '" + filename
           + "' cannot be loaded because it has not been found.");
    }
    fclose(file);
  }

  filename = load_filename;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void XMLFileLoader::validate_file(std::string schema_file) {

  xmlRelaxNGParserCtxtPtr ctxt = xmlRelaxNGNewParserCtxt(schema_file.c_str());
  if (NULL == ctxt)
    throw CycParseException("Failed to generate parser from schema: " + schema_file);

  xmlRelaxNGPtr schema = xmlRelaxNGParse(ctxt);

  xmlRelaxNGValidCtxtPtr vctxt = xmlRelaxNGNewValidCtxt(schema);

  doc = xmlReadFile(filename.c_str(), NULL,0);
  if (NULL == doc) {
    throw CycParseException("Failed to parse: " + filename);
  }

  if (xmlRelaxNGValidateDoc(vctxt,doc))
    throw CycParseException("Invalid XML file; file: "    
      + filename 
      + " does not validate against schema " 
      + schema_file);

  /// free up some data ???

}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void XMLFileLoader::load_catalog(std::string catalogElement, CatalogType catalogType, 
				 std::string cur_ns) {

  XMLQueryEngine xqe(catalogElement);

  string cat_filename, ns, format;
  cat_filename = xqe.get_content("filename");
  ns = xqe.get_content("namespace");
  format = xqe.get_content("format");
  
  if ("xml" == format) {
    CLOG(LEV_DEBUG2) << "going into a catalog...";
    XMLFileLoader catalog(cat_filename);
    // choose load function based on catalog type
    switch (catalogType) {
    case recipeBook:
      catalog.validate_file(recipe_book_schema_);
      catalog.load_recipes(cur_ns + ns + ":");
      break;
    case facilityCatalog:
      catalog.validate_file(facility_catalog_schema_);
      catalog.load_facilities(cur_ns + ns + ":");
      break;
    }
  } else {
    throw 
      CycRangeException(format + 
                        "is not a supported catalog format.");
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void XMLFileLoader::load_recipes(std::string cur_ns) {
  XMLQueryEngine xqe(doc);

  int numRecipeBooks = xqe.numElementsMatchingQuery("/*/recipebook");
  for (int rb_num=0;rb_num<numRecipeBooks;rb_num++) {
    load_catalog(xqe.get_content(rb_num),recipeBook,cur_ns);
  }

  string query = "/*/recipe";
  int numRecipes = xqe.numElementsMatchingQuery(query);
  for (int i=0; i<numRecipes; i++) {
    QueryEngine* qe = xqe.queryElement(query,i);
    RecipeLibrary::load_recipe(qe);
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void XMLFileLoader::load_facilities(std::string cur_ns) {
  XMLQueryEngine xqe(doc);

  int numFacCats = xqe.numElementsMatchingQuery("/*/facilitycatalog");
  for (int fac_cat_num=0;fac_cat_num<numFacCats;fac_cat_num++) {
    load_catalog(xqe.get_content(fac_cat_num),facilityCatalog,cur_ns);
  }
  load_models("/*/facility","Facility");
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void XMLFileLoader::load_all_models() {
  load_models("/*/converter","Converter");
  load_models("/*/market","Market");
  load_facilities("");
  load_models("/simulation/region","Region");
  load_models("/simulation/region/institution","Inst");
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void XMLFileLoader::load_models(std::string modelPath, std::string factoryType) {
  XMLQueryEngine xqe(doc);

  int numModels = xqe.numElementsMatchingQuery(modelPath.c_str());
  for (int model_num=0;model_num<numModels;model_num++) {
    //Model::create(factoryType,xqe.get_content(model_num));
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void XMLFileLoader::load_params() {
  XMLQueryEngine xqe(doc);

  string query = "/*/control";
  QueryEngine* qe = xqe.queryElement(query);
  TI->load_simulation(qe);
}
  
