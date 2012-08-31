// XMLQueryEngine.cpp
// Implements class for querying XML snippets
#include <iostream>
#include <sstream>

#include "XMLQueryEngine.h"

#include "CycException.h"

using namespace std;
using namespace boost;
using namespace xmlpp;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
XMLQueryEngine::XMLQueryEngine(XMLParser& parser) {
  current_node_ = parser.document()->get_root_node();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
XMLQueryEngine::XMLQueryEngine(xmlpp::Node* node) {
  current_node_ = node;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
int XMLQueryEngine::nElements() {
  int n = 0;
  const Node::NodeList nodelist = current_node_->get_children();  
  Node::NodeList::const_iterator it;
  for (it = nodelist.begin(); it != nodelist.end(); it++) {
    const Element* element = dynamic_cast<const Element*>(*it);
    if (element) 
      n++;
  }
  return n;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
int XMLQueryEngine::nElementsMatchingQuery(std::string query) {
  const NodeSet nodeset = current_node_->find(query);
  return nodeset.size();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
std::string XMLQueryEngine::getElementContent(std::string query,
                                              int index) {
  const NodeSet nodeset = current_node_->find(query);

  if (nodeset.size() < index+1)
    throw CycIndexException("Index exceeds number of nodes in query: " 
                            + query);

  const Element* element = 
    dynamic_cast<const Element*>(nodeset.at(index));

  if (!element) 
    throw CycNodeTypeException("Node: " + element->get_name() +
                               " is not an Element node.");

  const Node::NodeList nodelist = element->get_children();
  if (nodelist.size() != 1) 
    throw CycRangeException("Element node " + element->get_name() +
                            " has more content than expected.");
  
  const TextNode* text = 
    dynamic_cast<const xmlpp::TextNode*>(element->get_children().front());
  
  if (!text)
    throw CycNodeTypeException("Node: " + text->get_name() +
                               " is not a Text node.");
  
  return text->get_content();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
std::string XMLQueryEngine::getElementName(int index) {
  vector<Element*> elements;
  const Node::NodeList nodelist = current_node_->get_children();
  Node::NodeList::const_iterator it;
  for (it = nodelist.begin(); it != nodelist.end(); it++) {
    Element* element = dynamic_cast<Element*>(*it);
    if (element) 
      elements.push_back(element);
  }
  if (elements.size() < index+1)
    throw CycIndexException("Index exceeds number of elements in node: " 
                            + current_node_->get_name());
  return elements.at(index)->get_name();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
QueryEngine* XMLQueryEngine::getEngineFromQuery(std::string query,
                                                int index) {
  
  const NodeSet nodeset = current_node_->find(query);

  if (nodeset.size() < index+1)
    throw CycIndexException("Index exceeds number of nodes in query: " 
                            + query);

  Element* element = 
    dynamic_cast<Element*>(nodeset.at(index));

  if (!element) 
    throw CycNodeTypeException("Node: " + element->get_name() +
                               " is not an Element node.");

  return new XMLQueryEngine(element);
}
