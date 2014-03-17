// infile_tree.cc
// Implements class for querying XML snippets
#include <iostream>
#include <sstream>

#include "error.h"
#include "infile_tree.h"
#include <boost/lexical_cast.hpp>

namespace cyclus {

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
InfileTree::InfileTree(XMLParser& parser) : current_node_(0) {
  current_node_ = parser.Document()->get_root_node();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
InfileTree::InfileTree(xmlpp::Node* node) : current_node_(0) {
  current_node_ = node;
}

InfileTree::~InfileTree() {
  while (!spawned_children_.empty()) {
    InfileTree* qe_child = *spawned_children_.begin();
    spawned_children_.erase(spawned_children_.begin());
    if (qe_child) {
      delete qe_child;
    }
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int InfileTree::NElements() {
  using xmlpp::Element;
  int n = 0;
  const xmlpp::Node::NodeList nodelist = current_node_->get_children();
  xmlpp::Node::NodeList::const_iterator it;
  for (it = nodelist.begin(); it != nodelist.end(); it++) {
    const Element* element = dynamic_cast<const Element*>(*it);
    if (element) {
      n++;
    }
  }
  return n;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int InfileTree::NElementsMatchingQuery(std::string query) {
  return current_node_->find(query).size();
}

int InfileTree::GetInt(std::string query, int index) {
  return boost::lexical_cast<int>(GetString(query, index));
}

double InfileTree::GetDouble(std::string query, int index) {
  return boost::lexical_cast<double>(GetString(query, index));
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::string InfileTree::GetString(std::string query, int index) {
  using xmlpp::Node;
  using xmlpp::NodeSet;
  using xmlpp::TextNode;
  using xmlpp::Element;
  const NodeSet nodeset = current_node_->find(query);
  if (nodeset.empty()) {
    throw KeyError("Could not find a node by the name: " + query);
  }

  if (nodeset.size() < index + 1) {
    throw ValueError("Index exceeds number of nodes in query: " + query);
  }

  const Element* element =
    dynamic_cast<const Element*>(nodeset.at(index));

  if (!element) {
    throw CastError("Node: " + element->get_name() +
                    " is not an Element node.");
  }

  const Node::NodeList nodelist = element->get_children();
  if (nodelist.size() != 1) {
    throw ValueError("Element node " + element->get_name() +
                     " has more content than expected.");
  }

  const TextNode* text =
    dynamic_cast<const xmlpp::TextNode*>(element->get_children().front());

  if (!text) {
    throw CastError("Node: " + text->get_name() + " is not a Text node.");
  }

  return text->get_content();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::string InfileTree::GetElementName(int index) {
  using xmlpp::Node;
  using xmlpp::NodeSet;
  std::vector<xmlpp::Element*> elements;
  const Node::NodeList nodelist = current_node_->get_children();
  Node::NodeList::const_iterator it;
  for (it = nodelist.begin(); it != nodelist.end(); it++) {
    xmlpp::Element* element = dynamic_cast<xmlpp::Element*>(*it);
    if (element) {
      elements.push_back(element);
    }
  }
  if (elements.size() < index + 1) {
    throw ValueError("Index exceeds number of elements in node: "
                     + current_node_->get_name());
  }
  return elements.at(index)->get_name();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
InfileTree* InfileTree::GetEngineFromQuery(std::string query, int index) {
  using xmlpp::Node;
  using xmlpp::NodeSet;
  const NodeSet nodeset = current_node_->find(query);

  if (nodeset.size() < index + 1) {
    throw ValueError("Index exceeds number of nodes in query: " + query);
  }

  xmlpp::Element* element = dynamic_cast<xmlpp::Element*>(nodeset.at(index));

  if (!element) {
    throw CastError("Node: " + element->get_name() +
                    " is not an Element node.");
  }

  return new InfileTree(element);
}

InfileTree* InfileTree::QueryElement(std::string query,
                                       int index) {
  InfileTree* qe_child =
    GetEngineFromQuery(query, index);
  spawned_children_.insert(qe_child);
  return qe_child;
}
}  // namespace cyclus
