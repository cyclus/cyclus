//infile_tree.h
#ifndef CYCLUS_SRC_QUERY_ENGINE_H_
#define CYCLUS_SRC_QUERY_ENGINE_H_

#include <string>
#include <vector>
#include <set>

#include <libxml++/libxml++.h>
#include <boost/lexical_cast.hpp>

#include "xml_parser.h"

namespace cyclus {

/// @class InfileTree
///
/// A class for extracting information from a given XML parser
class InfileTree {
 public:
  /// constructor given a parser
  /// @param parser the xml parser
  InfileTree(XMLParser& parser);

  virtual ~InfileTree();

  /// @return the number of elements in the current query state
  virtual int NElements();

  /// investigates the current status and returns a string representing
  /// the name of a given index
  /// @param index the index of the queried element
  virtual std::string GetElementName(int index = 0);

  /// investigates the current status and returns the number of elements
  /// matching a query
  /// @param query the query
  /// @return the number of elements matching the query
  virtual int NMatches(std::string query);

  /// Same as GetString with auto-conversion to int.
  virtual int GetInt(std::string query, int index = 0);

  /// Same as GetString with auto-conversion to double.
  virtual double GetDouble(std::string query, int index = 0);

  /// investigates the current status and returns a string representing
  /// the content of a query at a given index
  /// @param query the query
  /// @param index the index of the queried element
  virtual std::string GetString(std::string query, int index = 0);

  /// populates a child query engine based on a query and index
  /// @param query the query
  /// @param index the index of the queried element
  /// @return a initialized query engine based on the query and index
  InfileTree* Query(std::string query, int index = 0);

 protected:
  /// constructor given a node
  /// @param node the node to set as the current node
  InfileTree(xmlpp::Node* node);

  /// every derived query engine must return a new instance initialized
  /// by a query.
  /// @param query the query
  /// @param index the index of the queried element
  /// @return a query engine initialized via the snippet
  virtual InfileTree* GetEngineFromQuery(std::string query, int index);

  /// sets the current node to a given node
  /// @param node the new current node
  void SetCurrentNode(xmlpp::Node* node);

 private:
  std::set<InfileTree*> spawned_children_;
  xmlpp::Node* current_node_;
};

/// @brief a query method for optional parameters
/// @param qe the query engine to use
/// @param query the query to be made
/// @param default_val the default value to use
/// @return either return the optional value if it exists or return the default
/// value
template <class T> inline T GetOptionalQuery(InfileTree* qe,
                                            std::string query,
                                            T default_val) {
  T val;
  qe->NMatches(query) == 1 ?
      val = boost::lexical_cast<T>(qe->GetString(query).c_str()) :
      val = default_val;
  return val;
};
  
}  // namespace cyclus

#endif  // CYCLUS_SRC_QUERY_ENGINE_H_
