//XMLQueryEngine.h
#if !defined(_XMLQUERYENGINE_H)
#define _XMLQUERYENGINE_H

#include <string>

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
     This most common constructor creates a set of XML objects from a
     snippet of XML text.
     
     @param snippet a string containing well-formed XML
  */
  XMLQueryEngine(std::string snippet);

  /// virtual destructor
  virtual ~XMLQueryEngine() {};
    
  /**
     @return the number of elements in the current query state
   */
  virtual int nElements();

  /**
     investigates the current status and returns a string representing
     the name of a given index
     @param index the index of the queried element
   */
  virtual std::string getElementName(int index = 0);

  /**
     investigates the current status and returns the number of elements
     matching a query
     @param query the query
     @return the number of elements matching the query
   */
  virtual int nElementsMatchingQuery(std::string query);
  
  /**
     investigates the current status and returns a string representing
     the content of a query at a given index
     @param query the query
     @param index the index of the queried element
   */
  virtual std::string getElementContent(std::string query, 
                                        int index = 0);

 protected:
  /**
     every derived query engine must return a new instance initialized
     by a query.
     @param query the query
     @param index the index of the queried element
     @return a query engine initialized via the snippet
   */
  virtual QueryEngine* getEngineFromQuery(std::string query,
                                          int index);
};

#include "CycException.h"
/**
   An exception class for an xpath that can not be evaluated
*/
class CycXPathException : public CycNullQueryException {
 public: 
 CycXPathException(std::string msg) : CycNullQueryException(msg) {};
};
#endif
