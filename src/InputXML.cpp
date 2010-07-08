// InputXML.cpp
// Implements XML input handling class
#include <iostream>

#include "InputXML.h"

#include "GenException.h"
#include "Logician.h"
#include "Model.h"

InputXML* InputXML::_instance = 0;

InputXML* InputXML::Instance() {

    if (0 == _instance)
	_instance = new InputXML();
    
    return _instance;

}

void InputXML::load_file(const char* filename)
{
    string err_msg;
    xmlFilename = filename;

    ctxt = xmlRelaxNGNewParserCtxt("file:///media/CYCLUS-DEV/cyclus/gc-full/branches/paul-branch/src/cyclus.ng.xsd");
    if (NULL == ctxt)
	throw GenException("Failed to generate parser.");

    xmlRelaxNGPtr schema = xmlRelaxNGParse(ctxt);

    xmlRelaxNGValidCtxtPtr vctxt = xmlRelaxNGNewValidCtxt(schema);

    doc = xmlReadFile(xmlFilename.c_str(), NULL,0);
    if (NULL == doc) {
	err_msg = "Failed to parse ";
	err_msg += xmlFilename;
	throw GenException(err_msg);
    }

    if (xmlRelaxNGValidateDoc(vctxt,doc))
	throw GenException("Invalid XML file.");
    else
	cerr << "VALID!!" << endl;

    Commodity::load_commodities();

    Model::load_markets();
    Model::load_facilities();
    Model::load_regions();

    Material::load_recipes();

}

xmlNodeSetPtr InputXML::get_elements(const char* expression)
{

    /* Create xpath evaluation context */
    xpathCtx = xmlXPathNewContext(doc);
    if(xpathCtx == NULL) {
        fprintf(stderr,"Error: unable to create new xpath context \n");
    }
    
    /* Evaluate xpath expression */
    xpathObj = xmlXPathEvalExpression((const xmlChar*)expression, xpathCtx);
    if(xpathObj == NULL) {
        fprintf(stderr,"Error: unable to evaluate xpath expression \"%s\"\n", expression);
        xmlXPathFreeContext(xpathCtx); 
    }

    return xpathObj->nodesetval;


    // when and how to cleanup memory allocation?

}

xmlNodeSetPtr InputXML::get_elements(xmlNodePtr cur,const char* expression)
{


    xpathCtx->node = cur;
    
    /* Evaluate xpath expression */
    xpathObj = xmlXPathEvalExpression((const xmlChar*)expression, xpathCtx);
    if(xpathObj == NULL) {
        fprintf(stderr,"Error: unable to evaluate xpath expression \"%s\"\n", expression);
        xmlXPathFreeContext(xpathCtx); 
    }

    return xpathObj->nodesetval;


    // when and how to cleanup memory allocation?

}
