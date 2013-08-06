// XMLParserTests.h
#include <gtest/gtest.h>

#include "XMLParser.h"

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
