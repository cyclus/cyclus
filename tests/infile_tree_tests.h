// infile_tree_tests.h
#ifndef CYCLUS_TESTS_INFILE_TREE_TESTS_H_
#define CYCLUS_TESTS_INFILE_TREE_TESTS_H_

#include <gtest/gtest.h>

#include <string>
#include <sstream>

#include "infile_tree.h"
#include "xml_parser.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class InfileTreeTest : public ::testing::Test {
 public:
  virtual void SetUp();
  virtual void TearDown();

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
};

#endif  // CYCLUS_TESTS_INFILE_TREE_TESTS_H_
