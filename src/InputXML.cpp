// InputXML.cpp
// Implements XML input handling class
#include <iostream>
#include <string.h>
#include <sys/stat.h>

#include "InputXML.h"

#include "Timer.h"
#include "Env.h"
#include "GenException.h"
#include "Logician.h"
#include "Model.h"

InputXML* InputXML::_instance = 0;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
string InputXML::main_schema = ENV->searchPathForFile("cyclus.rng","", ENV->checkEnv("CYCLUS_SRC_DIR"),"");

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
InputXML::InputXML()
{

  cur_ns = "";

}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
InputXML* InputXML::Instance() {

  if (0 == _instance)
  _instance = new InputXML();
  
  return _instance;

}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
xmlDocPtr InputXML::validate_file(xmlFileInfo *fileInfo)
{
  xmlRelaxNGParserCtxtPtr ctxt = xmlRelaxNGNewParserCtxt(fileInfo->schema->c_str());
  if (NULL == ctxt)
  throw GenException("Failed to generate parser from schema: " + *(fileInfo->schema));

  xmlRelaxNGPtr schema = xmlRelaxNGParse(ctxt);

  xmlRelaxNGValidCtxtPtr vctxt = xmlRelaxNGNewValidCtxt(schema);

  xmlDocPtr doc = xmlReadFile(fileInfo->filename.c_str(), NULL,0);
  if (NULL == doc) {
    throw GenException("Failed to parse: " + fileInfo->filename);
  }

  if (xmlRelaxNGValidateDoc(vctxt,doc))
    throw GenException("Invalid XML file; file: "    
      + fileInfo->filename 
      + " does not validate against schema " 
      + *(fileInfo->schema));
  else
    cerr << "File " << fileInfo->filename << " is valid against schema "
    << *(fileInfo->schema) << endl;

  /// free up some data

  return doc;

}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void InputXML::stripCurNS()
{

  if ("" == cur_ns)
  throw GenException("Unable to strip tokens from an empty namespace.");

  string::iterator pos = cur_ns.end();
  cur_ns.erase(--pos);

  size_t delimeter_pos = cur_ns.rfind(':');

  if (string::npos != delimeter_pos)
  cur_ns.erase(delimeter_pos);
  else
  cur_ns.erase();


}



//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void InputXML::load_file(string filename)
{
  curFilePtr = new xmlFileInfo;

  xmlFileInfo &inputFile = *curFilePtr;

  inputFile.filename = filename;
  inputFile.schema = &main_schema;
  inputFile.doc = validate_file(&inputFile);

  /* Create xpath evaluation context */
  inputFile.xpathCtxt = xmlXPathNewContext(inputFile.doc);
  if(inputFile.xpathCtxt == NULL) {
    fprintf(stderr,"Error: unable to create new xpath context \n");
  }
  
  Commodity::load_commodities();

  Material::load_recipes();
  
  Model::load_converters();
  Model::load_markets();
  Model::load_facilities();
  Model::load_regions();
  Model::load_institutions();

  TI->load_simulation();

  // delete/free mem

}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void InputXML::load_recipebook(string filename)
{
  /// store parent file info
  fileStack.push(curFilePtr);

  curFilePtr = new xmlFileInfo;
  xmlFileInfo &recipebook = *curFilePtr;

  recipebook.filename = filename;
  recipebook.schema = &main_schema;
  recipebook.doc = validate_file(&recipebook);
  recipebook.xpathCtxt = xmlXPathNewContext(recipebook.doc);

  Material::load_recipes();

  // get rid of recipebook, freeing memory
  delete curFilePtr;

  /// restore parent file info
  curFilePtr = fileStack.top();
  fileStack.pop();


}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void InputXML::load_facilitycatalog(string filename)
{
  /// store parent file info
  fileStack.push(curFilePtr);

  curFilePtr = new xmlFileInfo;
  xmlFileInfo &facilitycatalog = *curFilePtr;

  facilitycatalog.filename = filename;
  facilitycatalog.schema = &main_schema;
  facilitycatalog.doc = validate_file(&facilitycatalog);
  facilitycatalog.xpathCtxt = xmlXPathNewContext(facilitycatalog.doc);

  /// load here???
  Model::load_facilities();

  // get rid of facilitycatalog, freeing memory
  delete curFilePtr;

  /// restore parent file info
  curFilePtr = fileStack.top();
  fileStack.pop();


}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/*xmlNodeSetPtr InputXML::get_xpath_axes(xmlNodePtr cur,const char* expression)
{

  xmlXPathContextPtr xpathCtxt = curFilePtr->xpathCtxt;
  xpathCtxt->node = cur;
  
  xmlXPathObjectPtr xpathAx = xmlXPathEvalExpression((const xmlChar*)expression, xpathCtxt);
  if(xpathAx == NULL) {
    fprintf(stderr,"Error: unable to evaluate xpath expression \"%s\"\n", expression);
    xmlXPathFreeContext(xpathCtxt); 
  }

  return xpathAx->nodeaxes;
}
*/
xmlNodeSetPtr InputXML::get_xpath_elements(xmlNodePtr cur,const char* expression)
{

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
xmlNodePtr InputXML::get_xpath_element(xmlNodePtr cur,const char* expression)
{

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

const char* InputXML::get_xpath_content(xmlNodePtr cur,const char* expression)
{

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
const char* InputXML::get_xpath_name(xmlNodePtr cur,const char* expression)
{

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


