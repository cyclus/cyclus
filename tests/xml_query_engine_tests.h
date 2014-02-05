// xml_query_engine_tests.h
#ifndef CYCLUS_TESTS_XML_QUERY_ENGINE_TESTS_H_
#define CYCLUS_TESTS_XML_QUERY_ENGINE_TESTS_H_

#include <gtest/gtest.h>

#include <string>
#include <sstream>

#include "xml_query_engine.h"
#include "xml_parser.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class XMLQueryEngineTest : public ::testing::Test {
 protected:
  // equality checks
  std::string root_node_, content_node_, inner_node_, unknown_node_;
  std::string content_;
  int ncontent_;
  int ninner_nodes_;

  // loading helpers
  cyclus::XMLParser* parser_;
  void LoadParser();
  void GetContent(std::stringstream &ss);
  std::string Subcontent();
  std::string Unknowncontent();

 public:
  virtual void SetUp();
  virtual void TearDown();

}; 

#endif  // CYCLUS_TESTS_XML_QUERY_ENGINE_TESTS_H_
