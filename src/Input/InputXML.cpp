// InputXML.cpp
// Implements XML input handling class
#include <iostream>
#include <string>
#include <sys/stat.h>

#include "InputXML.h"

#include "Timer.h"
#include "Env.h"
#include "CycException.h"
#include "Model.h"
#include "Material.h"
#include "Logger.h"

InputXML* InputXML::instance_ = 0;

using namespace std;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
string InputXML::main_schema_ = "./Data/cyclus.rng";

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
InputXML::InputXML() {
  cur_ns_ = "";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
InputXML* InputXML::Instance() {
  main_schema_ = Env::getCyclusPath() + "/Data/cyclus.rng";

  if (0 == instance_)
  instance_ = new InputXML();
  
  return instance_;

}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
xmlDocPtr InputXML::validate_file(xmlFileInfo *fileInfo) {
  xmlRelaxNGParserCtxtPtr ctxt = xmlRelaxNGNewParserCtxt(fileInfo->schema->c_str());
  if (NULL == ctxt)
  throw CycParseException("Failed to generate parser from schema: " + *(fileInfo->schema));

  xmlRelaxNGPtr schema = xmlRelaxNGParse(ctxt);

  xmlRelaxNGValidCtxtPtr vctxt = xmlRelaxNGNewValidCtxt(schema);

  xmlDocPtr doc = xmlReadFile(fileInfo->filename.c_str(), NULL,0);
  if (NULL == doc) {
    throw CycParseException("Failed to parse: " + fileInfo->filename);
  }

  if (xmlRelaxNGValidateDoc(vctxt,doc))
    throw CycParseException("Invalid XML file; file: "    
      + fileInfo->filename 
      + " does not validate against schema " 
      + *(fileInfo->schema));

  /// free up some data

  return doc;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void InputXML::stripCurNS() {

  if ("" == cur_ns_)
  throw CycParseException("Unable to strip tokens from an empty namespace.");

  string::iterator pos = cur_ns_.end();
  cur_ns_.erase(--pos);

  size_t delimeter_pos = cur_ns_.rfind(':');

  if (string::npos != delimeter_pos)
  cur_ns_.erase(delimeter_pos);
  else
  cur_ns_.erase();

}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void InputXML::load_file(std::string filename) {
  // double check that the file exists
  if(filename=="") {
    throw CycIOException("No input filename was given");
  } else { 
    FILE* file = fopen(filename.c_str(),"r");
    if (file == NULL) { 
      throw CycIOException("The file '" + filename
           + "' cannot be loaded because it has not been found.");
    }
    fclose(file);
  }

  curFilePtr = new xmlFileInfo;

  xmlFileInfo &inputFile = *curFilePtr;

  inputFile.filename = filename;
  inputFile.schema = &main_schema_;
  inputFile.doc = validate_file(&inputFile);

  /* Create xpath evaluation context */
  inputFile.xpathCtxt = xmlXPathNewContext(inputFile.doc);
  if (inputFile.xpathCtxt == NULL) {
    fprintf(stderr,"Error: unable to create new xpath context \n");
  }

  // Recipes
  LOG(LEV_DEBUG3, "none!") << "Begin loading recipes";
  IsoVector::load_recipes();
  LOG(LEV_DEBUG3, "none!") << "End loading recipes";
  
  //Models
  LOG(LEV_DEBUG3, "none!") << "Begin loading models";
  Model::load_models();
  LOG(LEV_DEBUG3, "none!") << "End loading models";

  TI->load_simulation();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void InputXML::load_recipebook(std::string filename) {
  /// store parent file info
  fileStack_.push(curFilePtr);

  curFilePtr = new xmlFileInfo;
  xmlFileInfo &recipebook = *curFilePtr;

  recipebook.filename = filename;
  recipebook.schema = &main_schema_;
  recipebook.doc = validate_file(&recipebook);
  recipebook.xpathCtxt = xmlXPathNewContext(recipebook.doc);

  IsoVector::load_recipes();

  // get rid of recipebook, freeing memory
  delete curFilePtr;

  /// restore parent file info
  curFilePtr = fileStack_.top();
  fileStack_.pop();


}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void InputXML::load_facilitycatalog(std::string filename) {
  /// store parent file info
  fileStack_.push(curFilePtr);

  curFilePtr = new xmlFileInfo;
  xmlFileInfo &facilitycatalog = *curFilePtr;

  facilitycatalog.filename = filename;
  facilitycatalog.schema = &main_schema_;
  facilitycatalog.doc = validate_file(&facilitycatalog);
  facilitycatalog.xpathCtxt = xmlXPathNewContext(facilitycatalog.doc);

  /// load here???
  LOG(LEV_DEBUG3, "none!") << "Begin loading models - facilities";
  Model::load_facilities();
  LOG(LEV_DEBUG3, "none!") << "End loading models - facilities";

  // get rid of facilitycatalog, freeing memory
  delete curFilePtr;

  /// restore parent file info
  curFilePtr = fileStack_.top();
  fileStack_.pop();

}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
xmlNodeSetPtr InputXML::get_xpath_elements(xmlNodePtr cur,const char* expression) {

  xmlXPathContextPtr xpathCtxt = curFilePtr->xpathCtxt;
  xpathCtxt->node = cur;
  
  /* Evaluate xpath expression */
  xmlXPathObjectPtr xpathObj = xmlXPathEvalExpression((const xmlChar*)expression, xpathCtxt);
  if(xpathObj == NULL) {
    fprintf(stderr,"Error: unable to evaluate xpath expression \"%s\"\n", expression);
    xmlXPathFreeContext(xpathCtxt); 
  }

  return xpathObj->nodesetval;

  // when and how to cleanup memory allocation?

}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
xmlNodePtr InputXML::get_xpath_element(xmlNodePtr cur,const char* expression) {

  xmlXPathContextPtr xpathCtxt = curFilePtr->xpathCtxt;
  xpathCtxt->node = cur;
  
  /* Evaluate xpath expression */
  xmlXPathObjectPtr xpathObj = xmlXPathEvalExpression((const xmlChar*)expression, xpathCtxt);
  if(xpathObj == NULL) {
    fprintf(stderr,"Error: unable to evaluate xpath expression \"%s\"\n", expression);
    xmlXPathFreeContext(xpathCtxt); 
  }

  return xpathObj->nodesetval->nodeTab[0];

  // when and how to cleanup memory allocation?

}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const char* InputXML::get_xpath_content(xmlNodePtr cur,const char* expression) {

  xmlXPathContextPtr xpathCtxt = curFilePtr->xpathCtxt;
  xpathCtxt->node = cur;
  
  /* Evaluate xpath expression */
  xmlXPathObjectPtr xpathObj = xmlXPathEvalExpression((const xmlChar*)expression, xpathCtxt);
  if(xpathObj == NULL) {
    fprintf(stderr,"Error: unable to evaluate xpath expression \"%s\"\n", expression);
    xmlXPathFreeContext(xpathCtxt); 
  }

  return (const char*)(xpathObj->nodesetval->nodeTab[0]->children->content);

  // when and how to cleanup memory allocation?

}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const char* InputXML::get_xpath_name(xmlNodePtr cur,const char* expression) {

  xmlXPathContextPtr xpathCtxt = curFilePtr->xpathCtxt;
  xpathCtxt->node = cur;
  
  /* Evaluate xpath expression */
  xmlXPathObjectPtr xpathObj = xmlXPathEvalExpression((const xmlChar*)expression, xpathCtxt);
  if(xpathObj == NULL) {
    fprintf(stderr,"Error: unable to evaluate xpath expression \"%s\"\n", expression);
    xmlXPathFreeContext(xpathCtxt); 
  }

  return (const char*)(xpathObj->nodesetval->nodeTab[0]->name);

  // when and how to cleanup memory allocation?

}


