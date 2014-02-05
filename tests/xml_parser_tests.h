// xml_parser_tests.h
#ifndef CYCLUS_TESTS_XML_PARSER_TESTS_H_
#define CYCLUS_TESTS_XML_PARSER_TESTS_H_

#include <gtest/gtest.h>

#include "xml_parser.h"

#include <sstream>

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class XMLParserTests : public ::testing::Test {
 protected:
  void FillSnippet(std::stringstream &ss);
  void FillBadSnippet(std::stringstream &ss);
  void FillSchema(std::stringstream &ss);
  std::string outer_node_, inner_node_;
  std::string inner_content_;
 public:
  virtual void SetUp();
  virtual void TearDown();
};

#endif  // CYCLUS_TESTS_XML_PARSER_TESTS_H_
