#include <gtest/gtest.h>

#include "XMLParserTests.h"
#include <iostream>

using namespace std;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void XMLParserTests::fillSnippet(std::stringstream &ss) {
  ss << "<" << outer_node_ << ">"
     << "<" << inner_node_ << ">" << inner_content_
     << "</" << inner_node_ << ">"
     << "</" << outer_node_ << ">";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void XMLParserTests::fillBadSnippet(std::stringstream &ss) {
  ss << "<" << outer_node_ << ">"
     << "</" << outer_node_ << ">";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void XMLParserTests::fillSchema(std::stringstream &ss) {
  ss << "<grammar xmlns=\"http://relaxng.org/ns/structure/1.0\"" << endl
     << "datatypeLibrary=\"http://www.w3.org/2001/XMLSchema-datatypes\">" << endl
     << "  " << "<start>" << endl
     << "  " << "<element name =\"" << outer_node_ << "\">" << endl
     << "  " << "  " << "<element name =\"" << inner_node_ << "\">" << endl
     << "  " << "    " << "<text/>" << endl
     << "  " << "  " << "</element>" << endl
     << "  " << "</element>" << endl
     << "  " << "</start>" << endl
     << "</grammar>";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void XMLParserTests::SetUp() {
  inner_node_ = "inside";
  outer_node_ = "outside";
  inner_content_ = "inside_content";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void XMLParserTests::TearDown() {}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(XMLParserTests,WithoutSchema) {  
  stringstream snippet("");
  fillSnippet(snippet);
  EXPECT_NO_THROW(XMLParser parser(snippet));
} 

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(XMLParserTests,WithSchema) {
  stringstream snippet("");
  fillSnippet(snippet);  
  stringstream schema("");
  fillSchema(schema);
  EXPECT_NO_THROW(XMLParser parser(snippet,schema));
} 

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(XMLParserTests,WithError) {
  stringstream snippet("");
  fillBadSnippet(snippet);  
  stringstream schema("");
  fillSchema(schema);
  EXPECT_THROW(XMLParser parser(snippet,schema), CycLoadXMLException);
} 
