// XMLQueryEngine.cpp
// Implements class for querying XML snippets
#include <iostream>

#include "XMLQueryEngine.h"

#include "CycException.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
XMLQueryEngine::XMLQueryEngine() {
  doc_ = NULL;
  xpathCtxt_ = NULL;
  numElements_ = 0;
}

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
  xmlNodePtr node = currentXpathObj_->nodesetval->nodeTab[elementNum];
  // xmlNodePtr child = node->children;
  std::string XMLcontent = "";
  xmlBufferPtr nodeBuffer = xmlBufferCreate();
  
  if (nodeBuffer) {
    int success = xmlNodeBufGetContent(nodeBuffer, node);
    if (0 == success) 
      XMLcontent = (const char*) xmlBufferContent(nodeBuffer);
    else
      throw CycParseException("Couldn't dump node");

  // while (NULL != child) {
  //   xmlNodeDump(nodeBuffer,doc,child,0,1);
  //   XMLcontent += xmlBufferContent(nodeBuffer);
  //   // switch (child->type) {
  //   // case XML_ELEMENT_NODE:
  //   //   xmlNodeDump(nodeBuffer,doc,child,0,1);
  //   //   XMLcontent += (const char*)(nodeBuffer->content);
  //   //   break;
  //   // case XML_TEXT_NODE:
  //   //   XMLcontent += (const char*)(child->content);
  //   //   break;
  //   // default:
  //   //   XMLcontent = "XMLQueryEngine does not currently handle nodes of this type";
  //   // }

  //   child = child->next;
  // }
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
std::string XMLQueryEngine::get_name(int elementNum) {
  std::string XMLname = 
    (const char*)(currentXpathObj_->nodesetval->
                  nodeTab[elementNum]->name);
  return XMLname;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
QueryEngine* XMLQueryEngine::getEngineFromSnippet(std::string snippet) {
  return new XMLQueryEngine(snippet);
}
