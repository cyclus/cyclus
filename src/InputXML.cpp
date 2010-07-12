// InputXML.cpp
// Implements XML input handling class
#include <iostream>

#include "InputXML.h"

#include "GenException.h"
#include "Logician.h"
#include "Model.h"

InputXML* InputXML::_instance = 0;

string InputXML::main_schema = "file:///media/CYCLUS-DEV/cyclus/gc-full/branches/paul-branch/src/cyclus.ng.xsd";
string InputXML::recipebook_schema = "file:///media/CYCLUS-DEV/cyclus/gc-full/branches/paul-branch/src/cyclus.recipebook.ng.xsd";


InputXML* InputXML::Instance() {

    if (0 == _instance)
	_instance = new InputXML();
    
    return _instance;

}

xmlDocPtr InputXML::validate_file(xmlFileInfo *fileInfo)
{
    string err_msg;

    xmlRelaxNGParserCtxtPtr ctxt = xmlRelaxNGNewParserCtxt(fileInfo->schema->c_str());
    if (NULL == ctxt)
	throw GenException("Failed to generate parser.");

    xmlRelaxNGPtr schema = xmlRelaxNGParse(ctxt);

    xmlRelaxNGValidCtxtPtr vctxt = xmlRelaxNGNewValidCtxt(schema);

    xmlDocPtr doc = xmlReadFile(fileInfo->filename.c_str(), NULL,0);
    if (NULL == doc) {
	err_msg = "Failed to parse ";
	err_msg += fileInfo->filename;
	throw GenException(err_msg);
    }

    if (xmlRelaxNGValidateDoc(vctxt,doc))
	throw GenException("Invalid XML file.");
    else
	cerr << "File " << fileInfo->filename << " is valid against schema "
	     << *(fileInfo->schema) << endl;

    /// free up some data

    return doc;

}


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
    
    Model::load_markets();
    Model::load_facilities();
    Model::load_regions();
    Model::load_institutions();

    Material::load_XML_recipes();

    // delete/free mem

}


void InputXML::load_recipebook(string filename)
{
    /// store parent file info
    fileStack.push(curFilePtr);

    curFilePtr = new xmlFileInfo;
    xmlFileInfo &recipebook = *curFilePtr;

    recipebook.filename = filename;
    recipebook.schema = &recipebook_schema;
    recipebook.doc = validate_file(&recipebook);
    recipebook.xpathCtxt = xmlXPathNewContext(recipebook.doc);

    Material::load_XML_recipes();

    // get rid of recipebook, freeing memory
    delete curFilePtr;

    /// restore parent file info
    curFilePtr = fileStack.top();
    fileStack.pop();


}

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


