#include <gtest/gtest.h>

#include "XMLParserTests.h"
#include "Error.h"
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
  ss << "<grammar xmlns=\"http://relaxng.org/ns/structure/1.0\"" << std::endl
     << "datatypeLibrary=\"http://www.w3.org/2001/XMLSchema-datatypes\">" << std::endl
     << "  " << "<start>" << std::endl
     << "  " << "<element name =\"" << outer_node_ << "\">" << std::endl
     << "  " << "  " << "<element name =\"" << inner_node_ << "\">" << std::endl
     << "  " << "    " << "<text/>" << std::endl
     << "  " << "  " << "</element>" << std::endl
     << "  " << "</element>" << std::endl
     << "  " << "</start>" << std::endl
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
  std::stringstream snippet("");
  fillSnippet(snippet);
  cyclus::XMLParser parser;
  EXPECT_NO_THROW(parser.init(snippet));
} 

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(XMLParserTests,WithSchema) {
  std::stringstream snippet("");
  fillSnippet(snippet);  
  stringstream schema("");
  fillSchema(schema);
  cyclus::XMLParser parser;
  EXPECT_NO_THROW(parser.init(snippet));
  EXPECT_NO_THROW(parser.validate(schema));
} 

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(XMLParserTests,WithError) {
  stringstream snippet("");
  fillBadSnippet(snippet);  
  std::stringstream schema("");
  fillSchema(schema);
  cyclus::XMLParser parser;
  EXPECT_NO_THROW(parser.init(snippet));
  EXPECT_THROW(parser.validate(schema), cyclus::ValidationError);
} 
