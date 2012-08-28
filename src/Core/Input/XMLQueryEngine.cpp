// XMLQueryEngine.cpp
// Implements class for querying XML snippets
#include <iostream>
#include <sstream>

#include "XMLQueryEngine.h"

#include "CycException.h"

using namespace std;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
XMLQueryEngine::XMLQueryEngine(std::string snippet) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
int XMLQueryEngine::nElements() {
  return 1;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
int XMLQueryEngine::nElementsMatchingQuery(std::string query) {
  return 1;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
std::string XMLQueryEngine::getElementContent(std::string query,
                                              int index) {
  return "";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
std::string XMLQueryEngine::getElementName(int index) {
  return "";
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
QueryEngine* XMLQueryEngine::getEngineFromQuery(std::string query,
                                                int index) {
  return new XMLQueryEngine(query);
}
