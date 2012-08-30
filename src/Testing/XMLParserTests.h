// XMLParserTests.h
#include <gtest/gtest.h>

#include "XMLParser.h"

#include <sstream>

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class XMLParserTests : public ::testing::Test {
 protected:
  void fillSnippet(std::stringstream &ss);
  void fillSchema(std::stringstream &ss);
  std::string outer_node_, inner_node_;
  std::string inner_content_;
 public:
  virtual void SetUp();
  virtual void TearDown();
};
