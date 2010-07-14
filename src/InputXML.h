// InputXML.h
#if !defined(_INPUTXML_H)
#define _INPUTXML_H

#include <string>
#include <stack>

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

    struct xmlFileInfo {
	string filename;    /// filename currently being processed
	string* schema;
	xmlDocPtr doc;    /// XML doc ptr for input file
	xmlXPathContextPtr xpathCtxt; /// XML XPath search context
    } *curFilePtr;

    stack<xmlFileInfo*> fileStack;

    xmlDocPtr validate_file(xmlFileInfo *fileInfo);

    static string main_schema;
    static string recipebook_schema;
    
    
public:
    
    /// method to return a pointer to the only instance
    static InputXML* Instance();
    
    void load_file(string filename);
    void load_recipebook(string filename);
    void load_facilitycatalog(string filename);
    
    /// get nodes that match absolute path
    const char* get_xpath_content(const char* expression)
    {
	return get_xpath_content(curFilePtr->doc->children,expression);
    };
    xmlNodeSetPtr get_xpath_elements(const char* expression)
    {
	return get_xpath_elements(curFilePtr->doc->children,expression);
    };
    xmlNodePtr get_xpath_element(const char* expression)
    {
	return get_xpath_element(curFilePtr->doc->children,expression);
    };


    /// get the contents of the single element with this expression
    const char* get_xpath_content(xmlNodePtr cur,const char* expression);
    /// get the name of the single element with this expression
    const char* get_xpath_name(xmlNodePtr cur,const char* expression);
    /// get nodes that match relative path
    xmlNodeSetPtr get_xpath_elements(xmlNodePtr cur,const char* expression);
    /// get a single node that matches the relative path
    xmlNodePtr get_xpath_element(xmlNodePtr cur, const char* expression);

};



#endif
