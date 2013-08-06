//XMLquery_engine.h
#if !defined(_XMLQUERYENGINE_H)
#define _XMLQUERYENGINE_H

#include <string>
#include <vector>

#include "query_engine.h"
#include "xml_parser.h"
#include <libxml++/libxml++.h>

namespace cyclus {

/**
   @class XMLQueryEngine

   A class for extracting information from a given XML parser
*/
class XMLQueryEngine : public QueryEngine {
 public:
  /**
     constructor given a parser
     @param parser the xml parser
  */
  XMLQueryEngine(XMLParser& parser);

  /// virtual destructor
  virtual ~XMLQueryEngine() {};
    
  /**
     @return the number of elements in the current query state
   */
  virtual int NElements();

  /**
     investigates the current status and returns a string representing
     the name of a given index
     @param index the index of the queried element
   */
  virtual std::string GetElementName(int index = 0);

  /**
     investigates the current status and returns the number of elements
     matching a query
     @param query the query
     @return the number of elements matching the query
   */
  virtual int NElementsMatchingQuery(std::string query);
  
  /**
     investigates the current status and returns a string representing
     the content of a query at a given index
     @param query the query
     @param index the index of the queried element
   */
  virtual std::string GetElementContent(std::string query, 
                                        int index = 0);

 protected:
  /**
     constructor given a node
     @param node the node to set as the current node
  */
  XMLQueryEngine(xmlpp::Node* node);

  /**
     every derived query engine must return a new instance initialized
     by a query.
     @param query the query
     @param index the index of the queried element
     @return a query engine initialized via the snippet
   */
  virtual QueryEngine* GetEngineFromQuery(std::string query,
                                          int index);
  /**
     sets the current node to a given node
     @param node the new current node
   */
  void SetCurrentNode(xmlpp::Node* node);

 private:
  xmlpp::Node* current_node_;
};
} // namespace cyclus

#endif
