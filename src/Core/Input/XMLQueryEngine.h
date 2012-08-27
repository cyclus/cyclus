//XMLQueryEngine.h
#if !defined(_XMLQUERYENGINE_H)
#define _XMLQUERYENGINE_H

#include <string>

#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>
#include <libxml/relaxng.h>

#include "QueryEngine.h"

/**
   @class XMLQueryEngine

   A class for extracting information from XML snippets

   This class provides a simple mechanism for retreiving information
   from an XML snippet using the XPath query infrastructure.

   A new object is formed from a std::string snippet of XML,
   immediately creating an XML document object based on that snippet
   and an XPath search context based on that document object.

   These objects retain some state information from each query.  When
   a query matches, the object itself stores the number of matches
   that were found and the lsit of matches as an xmlXPathObject.

   When performing an XPath search for a given string expression,
   there may be multiple matches and each match may have multiple
   children.  This class provides methods to get the full contents of
   each match, or each child of each match.
*/
class XMLQueryEngine : public QueryEngine {
 public:
  /** 
      This default constructor should rarely be used explicitly, but
      is useful to allow simple declarations.
  */
  XMLQueryEngine();

  /// virtual destructor
  virtual ~XMLQueryEngine() {};

  /** 
      This most common constructor creates a set of XML objects from a
      snippet of XML text.

      @param snippet a string containing well-formed XML
   */
  XMLQueryEngine(std::string snippet);

  /** 
      This constructor allows a new object to be created from an
      already existing xmlDocPtr.

      @param current_doc a XML document object that has been generated
          elsewhere
   */
  XMLQueryEngine(xmlDocPtr current_doc);
  
  /**
     This is the primary search interface.  The object retains some
     state information as a result of this query for an XML element:
     the number of matches and the list of matches.

     @param expression an XPath search expression relative to the root
     of the document
   */
  virtual int numElementsMatchingQuery(std::string query);

  /**
     Get the full contents of a specific match to the expression
     search, including all the children.

     @param elementNum the ordinal index of the element to be returned
  */
  std::string get_content(int elementNum=0);

  /**
     This is a short cut to get the contents of the first match to a
     search expression.  It is intended for use when only a single
     match is expected.

     @param expression an XPath search expression relative to the root
     of the document
  */
  std::string get_content(const char* expression);

  /**
   */
  virtual std::string getElementContent(std::string query,
                                        int index = 0);

  /**
     Get the number of children of a specific match to the expression
     search.

     @param elementNum the ordinal index of the element to be returned
  */
  int get_num_children(int elementNum=0);

  /**
     This is a short cut to get the number of children of the first
     match to a search expression.  It is intended for use when only a
     single match is expected.

     @param expression an XPath search expression relative to the root
     of the document
  */
  int get_num_children(const char* expression);

  /**
     Get the contents of a specific child element of all the matches
     to a search expression.

     @param elementNum the ordinal index of the element to be returned
     @param childNum the ordinal index of the child to be returned
  */
  std::string get_child(int elementNum=0, int childNum=0);
  
  /**
     This is a short cut to get the first child element of the first
     match to a search expression.  It is intended for use when only a
     single match is expected.

     @param expression an XPath search expression relative to the root
     of the document
   */
  std::string get_child(const char* expression);
  
  /**
     Get the name of a child element.

     @param elementNum the ordinal number of the element to be queried
  */
  std::string getElementName(int index=0);

 protected:
  virtual QueryEngine* getEngineFromSnippet(std::string snippet);

 private:
  int numElements_;
  xmlDocPtr doc_;
  xmlXPathContextPtr xpathCtxt_;
  xmlXPathObjectPtr currentXpathObj_;

  void init(std::string expression);


};

#include "CycException.h"
/**
   An exception class for an xpath that can not be evaluated
*/
class CycXPathException : public CycException {
 public: 
 CycXPathException(std::string msg) : CycException(msg) {};
};

#endif
