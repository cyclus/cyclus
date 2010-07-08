// InputXML.h
#if !defined(_INPUTXML_H)
#define _INPUTXML_H

#include <string>

using namespace std;

#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>
#include <libxml/relaxng.h>

#define XMLinput InputXML::Instance()

class InputXML
{

private:
    /// pointer to thie single instance of this class
    static InputXML* _instance;

    /// default constructor
    InputXML() {};

    /// default destructor
    ~InputXML() {};

    /// document tree populated by parser
    xmlDocPtr doc;
    
    /// parser context for validation
    xmlRelaxNGParserCtxtPtr ctxt;

    /// XPath context for searching
    xmlXPathContextPtr xpathCtx; 

    /// XPath object - results of searching
    xmlXPathObjectPtr xpathObj; 

    /// filename currently being processed
    string xmlFilename;

public:

    /// method to return a pointer to the only instance
    static InputXML* Instance();

    void load_file_valid(const char* filename) {};
    void load_file(const char* filename);

    /// get nodes that match absolute path
    xmlNodeSetPtr get_elements(const char* expression);
    /// get nodes that match relative path
    xmlNodeSetPtr get_elements(xmlNodePtr cur,const char* expression);

};



#endif
