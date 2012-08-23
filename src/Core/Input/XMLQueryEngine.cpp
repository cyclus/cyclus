// XMLQueryEngine.cpp
// Implements class for querying XML snippets

#include <iostream>

#include "XMLQueryEngine.h"

#include "CycException.h"

//- - - - - - 

XMLQueryEngine::XMLQueryEngine() {
  
  doc = NULL;
  xpathCtxt = NULL;
  numElements = 0;

}

XMLQueryEngine::XMLQueryEngine(std::string snippet) {

  init(snippet);

}

XMLQueryEngine::XMLQueryEngine(xmlDocPtr current_doc) {

  if (NULL == current_doc) {
    throw CycParseException("Invalide xmlDocPtr passed into XMLQueryEngine");
  }
  
  doc = current_doc;
  xpathCtxt = xmlXPathNewContext(doc);
  numElements = 0;

}

void XMLQueryEngine::init(std::string snippet) {

  char *myEncoding = NULL;
  int myParserOptions = 0;
  doc = xmlReadDoc((const xmlChar*)snippet.c_str(),"",myEncoding,myParserOptions);
  if (NULL == doc) {
    // throw CycParseException("Failed to parse snippet");
  }
  
  xpathCtxt = xmlXPathNewContext(doc);
  numElements = 0;

}


//- - - - - - - 
int XMLQueryEngine::find_elements(const char* expression) {

  numElements = 0;

  /* Evaluate xpath expression */
  currentXpathObj = xmlXPathEvalExpression((const xmlChar*)expression, xpathCtxt);
  
  if (NULL != currentXpathObj)
    numElements = currentXpathObj->nodesetval->nodeNr;

  return numElements;

}

//- - - - - -
std::string XMLQueryEngine::get_content(const char* expression) {

  if (0 == find_elements(expression)) {
    throw CycParseException("Can't find an element with that name.");
  }

  return get_content(0);
}

std::string XMLQueryEngine::get_content(int elementNum) {

  
  xmlNodePtr node = currentXpathObj->nodesetval->nodeTab[elementNum];
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

std::string XMLQueryEngine::get_child(const char* expression) {

  if (0 == find_elements(expression)) {
    throw CycParseException("Can't find an element with that name.");
  }
  
  return get_child(0);

}

std::string XMLQueryEngine::get_child(int elementNum, int childNum) {

  if (elementNum >= numElements) {
    throw CycParseException("Too many elements requested.");
  }

  xmlNodePtr node = currentXpathObj->nodesetval->nodeTab[elementNum];
  xmlNodePtr child = node->children;
  std::string XMLcontent;

  while (childNum-- > 0 && NULL != child)
    child = child->next;
  
  if (NULL == child)
    throw CycParseException("Too many children requested.");

  xmlBufferPtr nodeBuffer = xmlBufferCreate();
  if (nodeBuffer)
    {
      int success = xmlNodeDump(nodeBuffer,doc,child,0,1);
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

//- - - - - - 
std::string XMLQueryEngine::get_name(int elementNum) {

  std::string XMLname = (const char*)(currentXpathObj->nodesetval->nodeTab[elementNum]->name);

  return XMLname;

}

