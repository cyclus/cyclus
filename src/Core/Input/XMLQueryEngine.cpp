// XMLQueryEngine.cpp
// Implements class for querying XML snippets
#include <iostream>
#include <sstream>

#include "XMLQueryEngine.h"

#include "CycException.h"

using namespace std;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
XMLQueryEngine::XMLQueryEngine(std::string snippet) {
  init(snippet);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
XMLQueryEngine::XMLQueryEngine(xmlDocPtr current_doc) {
  if (NULL == current_doc) {
    throw CycParseException("Invalide xmlDocPtr passed into XMLQueryEngine");
  }
  
  doc_ = current_doc;
  xpathCtxt_ = xmlXPathNewContext(doc_);
  numElements_ = 0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void XMLQueryEngine::init(std::string snippet) {
  char *myEncoding = NULL;
  int myParserOptions = 0;
  doc_ = xmlReadDoc((const xmlChar*)snippet.c_str(),"",myEncoding,myParserOptions);
  if (NULL == doc_) {
    // throw CycParseException("Failed to parse snippet");
  }
  
  xpathCtxt_ = xmlXPathNewContext(doc_);
  numElements_ = 0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
int XMLQueryEngine::numElementsMatchingQuery(std::string query) {//const char* expression) {
  numElements_ = 0;

  /* Evaluate xpath expression */
  currentXpathObj_ = xmlXPathEvalExpression((const xmlChar*)query.c_str(), xpathCtxt_);
  
  if (!xmlXPathNodeSetIsEmpty(currentXpathObj_->nodesetval)) {
    numElements_ = currentXpathObj_->nodesetval->nodeNr;
  }

  return numElements_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
std::string XMLQueryEngine::getElementContent(std::string query,
                                              int index) {
  numElementsMatchingQuery(query);
  return get_content(index);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
std::string XMLQueryEngine::get_content(const char* expression) {
  if (0 == numElementsMatchingQuery(expression)) {
    throw CycParseException("Can't find an element with that name.");
  }

  return get_content(0);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
std::string XMLQueryEngine::get_content(int elementNum) {
  xmlNodeSetPtr nodeSet = currentXpathObj_->nodesetval;
  
  if(nodeSet->nodeNr < elementNum + 1) {
    stringstream ss("");
    ss << "Error: " << "query only has " << nodeSet->nodeNr 
       << " elements.";
    throw CycXPathException(ss.str());
  }

  xmlNodePtr node = nodeSet->nodeTab[elementNum];
  // xmlNodePtr child = node->children;
  std::string XMLcontent = "";
  xmlBufferPtr nodeBuffer = xmlBufferCreate();
  
  if (nodeBuffer) {
    int success = xmlNodeBufGetContent(nodeBuffer, node);
    if (0 == success) 
      XMLcontent = (const char*) xmlBufferContent(nodeBuffer);
    else
      throw CycParseException("Couldn't dump node");
  }
  else
    throw CycParseException("Couldn't allocate buffer.");
  
  xmlBufferFree(nodeBuffer);

  return XMLcontent;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
std::string XMLQueryEngine::get_child(const char* expression) {
  if (0 == numElementsMatchingQuery(expression)) {
    throw CycParseException("Can't find an element with that name.");
  }
  
  return get_child(0);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
std::string XMLQueryEngine::get_child(int elementNum, int childNum) {
  if (elementNum >= numElements_) {
    throw CycParseException("Too many elements requested.");
  }

  xmlNodePtr node = currentXpathObj_->nodesetval->nodeTab[elementNum];
  xmlNodePtr child = node->children;
  std::string XMLcontent;

  while (childNum-- > 0 && NULL != child)
    child = child->next;
  
  if (NULL == child)
    throw CycParseException("Too many children requested.");

  xmlBufferPtr nodeBuffer = xmlBufferCreate();
  if (nodeBuffer)
    {
      int success = xmlNodeDump(nodeBuffer,doc_,child,0,1);
      if (-1 < success)
	XMLcontent = (const char*)xmlBufferContent(nodeBuffer);
      else
	throw CycParseException("Couldn't dump node");
    }
  else
    throw CycParseException("Couldn't allocate buffer.");

  xmlBufferFree(nodeBuffer);

  return XMLcontent;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
std::string XMLQueryEngine::getElementName(int elementNum) {

  stringstream ss("");
  ss << elementNum;

  if (currentXpathObj_->nodesetval->nodeNr < elementNum) {
    throw CycParseException("Snippet does not have " 
                            + ss.str() + " elements.");
  }

  std::string XMLname = 
    (const char*)(currentXpathObj_->nodesetval->
                  nodeTab[elementNum]->name);
  return XMLname;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
QueryEngine* XMLQueryEngine::getEngineFromSnippet(std::string snippet) {
  return new XMLQueryEngine(snippet);
}
