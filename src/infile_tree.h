//infile_tree.h
#ifndef CYCLUS_SRC_INFILE_TREE_H_
#define CYCLUS_SRC_INFILE_TREE_H_

#include <string>
#include <vector>
#include <set>

#include <libxml++/libxml++.h>
#include <boost/algorithm/string.hpp>
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

  /// investigates the current status and returns a string representing
  /// the content of a query at a given index
  /// @param query the query
  /// @param index the index of the queried element
  virtual std::string GetString(std::string query, int index = 0);

  /// populates a child infile based on a query and index
  /// @param query the query
  /// @param index the index of the queried element
  /// @return a initialized infile based on the query and index
  InfileTree* SubTree(std::string query, int index = 0);

 protected:
  /// constructor given a node
  /// @param node the node to set as the current node
  InfileTree(xmlpp::Node* node);

  /// every derived infile must return a new instance initialized
  /// by a query.
  /// @param query the query
  /// @param index the index of the queried element
  /// @return a infile initialized via the snippet
  virtual InfileTree* GetEngineFromQuery(std::string query, int index);

  /// sets the current node to a given node
  /// @param node the new current node
  void SetCurrentNode(xmlpp::Node* node);

 private:
  std::set<InfileTree*> spawned_children_;
  xmlpp::Node* current_node_;
};
  
/// @brief a query method for required parameters
/// @param tree the infile tree to use
/// @param query the query to be made
/// @param index the index of the queried element
/// @return either return the optional value if it exists or return the default
/// value
template <typename T>
    inline T Query(InfileTree* tree, std::string query, int index = 0) {
  return boost::lexical_cast<T>(tree->GetString(query, index).c_str());
}

template <>
    inline std::string Query(InfileTree* tree, std::string query, int index) {
  return tree->GetString(query, index);
}

template <>
    inline int Query(InfileTree* tree, std::string query, int index) {
  std::string s = tree->GetString(query, index);
  boost::trim(s);
  return boost::lexical_cast<int>(s.c_str());
}

template <>
    inline float Query(InfileTree* tree, std::string query, int index) {
  std::string s = tree->GetString(query, index);
  boost::trim(s);
  return boost::lexical_cast<float>(s.c_str());
}

template <>
    inline double Query(InfileTree* tree, std::string query, int index) {
  std::string s = tree->GetString(query, index);
  boost::trim(s);
  return boost::lexical_cast<double>(s.c_str());
}

/// @brief a query method for optional parameters
/// @param tree the infile tree to use
/// @param query the query to be made
/// @param default_val the default value to use
/// @return either return the optional value if it exists or return the default
/// value
template <typename T>
    inline T OptionalQuery(InfileTree* tree, std::string query, T default_val) {
  T val;
  tree->NMatches(query) == 1 ?
    val = boost::lexical_cast<T>(tree->GetString(query).c_str()) :
    val = default_val;
  return val;
}
  
template <>
    inline int OptionalQuery(InfileTree* tree, std::string query, int default_val) {
  int val = default_val;
  if (tree->NMatches(query) == 1) {
    std::string s = tree->GetString(query);
    boost::trim(s);
    val = boost::lexical_cast<int>(s.c_str());
  }
  return val;
}
  
template <>
    inline float OptionalQuery(InfileTree* tree, std::string query, float default_val) {
  float val = default_val;
  if (tree->NMatches(query) == 1) {
    std::string s = tree->GetString(query);
    boost::trim(s);
    val = boost::lexical_cast<float>(s.c_str());
  }
  return val;
}

template <>
    inline double OptionalQuery(InfileTree* tree, std::string query, double default_val) {
  double val = default_val;
  if (tree->NMatches(query) == 1) {
    std::string s = tree->GetString(query);
    boost::trim(s);
    val = boost::lexical_cast<double>(s.c_str());
  }
  return val;
}

}  // namespace cyclus

#endif  // CYCLUS_SRC_INFILE_TREE_H_
