// InputXML.h
#if !defined(_INPUTXML_H)
#define _INPUTXML_H

#include <string>
#include <stack>

#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>
#include <libxml/relaxng.h>

#define XMLinput InputXML::Instance()

/**
   provide interface to libxml for all input parsing needs
   
   @section intro Introduction
   This class provides a convenient set of functions to validate, load,
   parse, search and extract data from an XML input file.  Features include
   RelaxNG schema validation and nested inclusion of libraries with
   disambiguating namespace modifiers.
   
   To facilitate the nested inclusion of libraries, a structure is defined to
   hold information about the current XML document.  These objects are placed
   on a stack to manage an infinite amount of nesting.
   
   This class is implemented as a singleton with preferred access through the
   pre-processor-defined XMLinput macro
 */

class InputXML {
public:
  /**
     Method to return a pointer to the only instance
   */ 
  static InputXML* Instance();

  /**
     Method to return the current namespace modifier
   */ 
  std::string getCurNS() { return cur_ns_; };

  /**
     Extend the current namespace modifier
     
     @param ns string to append to current namespace modifier
   */
  void extendCurNS(std::string ns) { cur_ns_ += ns + ":"; };

  /**
     Strip last namespace from namespace modifier
     
     This method assumes that for all cases other than a blank modifier,
     the namespace modifier is always delimited by a colon, including a
     final colon.  This strips off everything after the second-last colon.
   */
  void stripCurNS();

  /**
     Load an XML input file
     
     This method will open a file assign its information to an xmlFileInfo
     structure and validate the file against a given RelaxNG schema.  After
     these generic steps, the method will load the objects described by the
     XML file in a carefully defined order:
     
     
     
     
     
     
     @param filename name of the file to  load
   */
  void load_file(std::string filename);

  /**
     Opens and loads a recipebook: included library of recipes
     
     This method will push the current xmlFileInfo strcutre onto the stack
     and create a new one for the recipebook library to be opened.  It will
     be opened and validated against a Relax-NG schema and then the recipes
     will be loaded using the same method as used by load_file()
     
     @param filename name of recipebook file to open
   */
  void load_recipebook(std::string filename);

  /**
     Opens and loads a facilitycatalog: included catalog of facility
     definitions
     
     This method will push the current xmlFileInfo strcutre onto the stack
     and create a new one for the facilitycatalog library to be opened.  It
     will be opened and validated against a Relax-NG schema and then the
     facilities will be loaded using the same method as used by load_file()
     
     @param filename name of facilitycatalog file to open
   */
  void load_facilitycatalog(std::string filename);
  
  /**
     Get the contents of the single element with this XPath
     expression
     
     This method will return the contents of the first element in the XML
     document tree that matches this XPath expression.  Ideally, this
     should only be called when a single element exists, because no error
     will be issued when only the first match is returned.
     
     @param cur XML node pointer from which to start the search
     @param expression XPath expression that will be interpreted relative
     to the XML node pointer cur
   */
  const char* get_xpath_content(xmlNodePtr cur,const char* expression);

  /**
     Get content of single node from current XML doc that match
     XPath expression
     
     Uses the first XML node pointer in the XML doc pointer of the current
     xmlFileInfo object to call get_xpath_content()
     
     @param expression XPath search expression is either an absolute path
     or a path relative to the document root
     
   */
  const char* get_xpath_content(const char* expression)
  {
    return get_xpath_content(curFilePtr->doc->children,expression);
  };

  /**
     Get list of nodes that match relative XPath expression
     
     This method will return an xmlNodeSetPtr that provides a list of nodes
     that match the given XPath expression.
     
     @param cur XML node pointer from which to start the search 
     @param expression XPath expression that will be interpreted relative
     to the XML node pointer cur
   */
  xmlNodeSetPtr get_xpath_elements(xmlNodePtr cur,const char* expression);

  /**
     Get list of nodes from current XML doc that match XPath
     expression
     
     Uses the first XML node pointer in the XML doc pointer of the current
     xmlFileInfo object to call get_xpath_elements()
     
     @param expression XPath expression that will be interpreted relative
     to the XML node pointer cur
   */
  xmlNodeSetPtr get_xpath_elements(const char* expression) {
    return get_xpath_elements(curFilePtr->doc->children,expression);
  };

  /**
     Get a single node that matches relative XPath expression
     
     This method will return an xmlNodePtr to the single node that matches
     the given XPath expression.
     
     @param cur XML node pointer from which to start the search 
     @param expression XPath expression that will be interpreted relative
     to the XML node pointer cur
   */
  xmlNodePtr get_xpath_element(xmlNodePtr cur, const char* expression);

  /**
     Get a single node from current XML doc that matches XPath
     expression
     
     Uses the first XML node pointer in the XML doc pointer of the current
     xmlFileInfo object to call get_xpath_elements()
     
     @param expression XPath expression that will be interpreted relative
     to the XML node pointer cur
   */
  xmlNodePtr get_xpath_element(const char* expression) {
    return get_xpath_element(curFilePtr->doc->children,expression);
  };

  /**
     Get the name of the single element with this XPath expression
     
     This method will return the name of the first element in the XML
     document tree that matches this XPath expression.  Ideally, this
     should only be called when a single element exists, because no error
     will be issued when only the first match is returned.
     
     This is generally used with wildcard expressions since the name would
     not already be known.  If a fixed expression is used, then the name is
     probably already known and this method doesn't need to be called.
     
     @param cur XML node pointer from which to start the search
     @param expression XPath expression that will be interpreted relative
     to the XML node pointer cur
   */
  const char* get_xpath_name(xmlNodePtr cur,const char* expression);

private:
  /**
     pointer to thie single instance of this class
   */ 
  static InputXML* instance_;

  /**
     default constructor
   */ 
  InputXML();

  /**
     default destructor
   */ 
  ~InputXML() {};

  /**
     A structure to contain related info about an XML file
     
     This structure is used to create an association between a filename,
     the schema used to validate it, the XML doc ptr to that file and an
     existing XPath search context.
   */
  struct xmlFileInfo {
    /**
       filename currently being processed
     */ 
    std::string filename;  

    /**
       pointer to the schema URI used for this file
     */ 
    std::string* schema;   

    /**
       XML doc ptr for input file
     */ 
    xmlDocPtr doc;    

    /**
       XML XPath search context
     */ 
    xmlXPathContextPtr xpathCtxt; 
  } 

  /**
     pointer to record of the current file being processed
   */ 
  *curFilePtr;

  /**
     Stack of information about nexted input files
     
     Every time a nested input file is openned, the current fileinfo
     is pushed onto this stack.  Every time a nested input file is closed
     the last fileinfo is popped off this stack.
   */
  std::stack<xmlFileInfo*> fileStack_;

  /**
     Current namespace modifier value
     
     To reduce the risk of naming conflicts with included files, a
     namespace modifier is used in the naming of ojbects loaded from
     incldued files.  At the point of inclusion in the parent file, the
     namespace extension for that file is defined by the user.  For
     top-level objects the namespace modifier is blank.  For all other
     objects, the namespace modifier is a colon-delimited list of strings
     with a trailing colon.
   */
  std::string cur_ns_;

  /**
     Validate an XML file against a Relax-NG schema
     
     Open and validate that a file matches the grammar defined in the
     Relax-NG schema that has been assocaited with the file in the fileinfo
     record.  
     
     @param fileInfo xmlFileInfo record that associates this file with a
     schema
   */
  xmlDocPtr validate_file(xmlFileInfo *fileInfo);

  /**
     primary schema used for Cyclus processing
   */ 
  static std::string main_schema_; 

};



#endif
