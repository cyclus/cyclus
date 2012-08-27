// InputXML.cpp
// Implements XML input handling class
#include <iostream>
#include <string>
#include <sys/stat.h>

#include "InputXML.h"
#include "XMLQueryEngine.h"

#include "Timer.h"
#include "Env.h"
#include "RecipeLibrary.h"
#include "CycException.h"
#include "Model.h"
//#include "Material.h"
#include "Logger.h"

InputXML* InputXML::instance_ = 0;

using namespace std;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
string InputXML::main_schema_ = "/share/cyclus.rng";

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
InputXML::InputXML() {
  cur_ns_ = "";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
InputXML* InputXML::Instance() {
  main_schema_ = Env::getInstallPath() + "/share/cyclus.rng";

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

  xmlInputFile = XMLFileLoader(filename);

  xmlInputFile.validate_file(XMLFileLoader::main_schema_);

  // timer sets data
  initializeSimulationTimeData();

  // recipes, markets, converters, prototypes
  loadGlobalSimulationElements();

  // regions, institutions
  loadSimulationEntities();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void InputXML::loadGlobalSimulationElements() {
  // Recipes
  LOG(LEV_DEBUG3, "none!") << "Begin loading recipes";
  xmlInputFile.load_recipes("");
  LOG(LEV_DEBUG3, "none!") << "End loading recipes";

  //Models
  LOG(LEV_DEBUG3, "none!") << "Begin loading elements";
  xmlInputFile.load_all_models();
  LOG(LEV_DEBUG3, "none!") << "End loading elements";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void InputXML::loadSimulationEntities() {
  LOG(LEV_DEBUG3, "none!") << "Begin loading entities";
  //Model::loadEntities();
  LOG(LEV_DEBUG3, "none!") << "End loading entities";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void InputXML::initializeSimulationTimeData() {
  xmlInputFile.load_params();
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
  //Model::load_facilities();
  LOG(LEV_DEBUG3, "none!") << "End loading models - facilities";

  // get rid of facilitycatalog, freeing memory
  delete curFilePtr;

  /// restore parent file info
  curFilePtr = fileStack_.top();
  fileStack_.pop();

}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
xmlNodeSetPtr InputXML::get_xpath_elements(xmlXPathContextPtr& context,
                                           xmlNodePtr& cur,
                                           const char* expression) {
  context->node = cur;
  
  /* Evaluate xpath expression */
  xmlXPathObjectPtr xpathObj = 
    xmlXPathEvalExpression((const xmlChar*)expression, context);

  if(xmlXPathNodeSetIsEmpty(xpathObj->nodesetval)) {
    stringstream ss("");
    ss << "Error: unable to evaluate xpath expression " 
       << expression;
    throw CycNullXPathException(ss.str());
  }

  return xpathObj->nodesetval;

  // when and how to cleanup memory allocation?

}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
xmlNodePtr InputXML::get_xpath_element(xmlXPathContextPtr& context,
                                       xmlNodePtr& cur,
                                       const char* expression) {
  context->node = cur;
  
  /* Evaluate xpath expression */
  xmlXPathObjectPtr xpathObj = 
    xmlXPathEvalExpression((const xmlChar*)expression,context);

  if(xmlXPathNodeSetIsEmpty(xpathObj->nodesetval)) {
    stringstream ss("");
    ss << "Error: unable to evaluate xpath expression " 
       << expression;
    throw CycNullXPathException(ss.str());
  }
  
  return xpathObj->nodesetval->nodeTab[0];

  // when and how to cleanup memory allocation?

}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const char* InputXML::get_xpath_content(xmlXPathContextPtr& context,
                                        xmlNodePtr& cur,
                                        const char* expression) {
  context->node = cur;
  
  /* Evaluate xpath expression */
  xmlXPathObjectPtr xpathObj = 
    xmlXPathEvalExpression((const xmlChar*)expression, context);
  
  if(xmlXPathNodeSetIsEmpty(xpathObj->nodesetval)) {
    stringstream ss("");
    ss << "Error: unable to evaluate xpath expression " 
       << expression;
    throw CycNullXPathException(ss.str());
  }

  return (const char*)(xpathObj->nodesetval->
                       nodeTab[0]->children->content);

  // when and how to cleanup memory allocation?

}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const char* InputXML::get_xpath_name(xmlXPathContextPtr& context,
                                     xmlNodePtr& cur,
                                     const char* expression) {
  context->node = cur;
  
  /* Evaluate xpath expression */
  xmlXPathObjectPtr xpathObj = 
    xmlXPathEvalExpression((const xmlChar*)expression, context);

  if(xmlXPathNodeSetIsEmpty(xpathObj->nodesetval)) {
    stringstream ss("");
    ss << "Error: unable to evaluate xpath expression " 
       << expression;
    throw CycNullXPathException(ss.str());
  }

  return (const char*)(xpathObj->nodesetval->nodeTab[0]->name);

  // when and how to cleanup memory allocation?

}


