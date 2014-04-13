// infile_tree_tests.cc
#include "infile_tree_tests.h"

#include <iostream>
#include "xml_parser.h"
#include "error.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::string InfileTreeTest::Unknowncontent() {
  std::stringstream ss("");
  ss << "    <" << unknown_node_ << ">" << std::endl
     << "      <" << content_node_ << ">" << content_ << "</" << content_node_ << ">" << std::endl
     << "    </" << unknown_node_ << ">" << std::endl;
  return ss.str();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::string InfileTreeTest::Subcontent() {
  std::stringstream ss("");
  ss  << "  <" << content_node_ << ">" << content_ << "</" << content_node_ << ">" << std::endl;
  return ss.str();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void InfileTreeTest::GetContent(std::stringstream &ss) {
  ss << "<" << root_node_ << ">" << std::endl;
  for (int i = 0; i < ncontent_; i++) {
    ss << Subcontent();
  }
  ss << "  <" << inner_node_ << ">"  << std::endl
     << Unknowncontent()
     << "  </" << inner_node_ << ">" << std::endl
     << "</" << root_node_ << ">";
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void InfileTreeTest::SetUp() {
  root_node_ = "facility";
  content_node_ = "name";
  content_ = "the_name";
  inner_node_ = "module";
  unknown_node_ = "unknown_module_name";
  ncontent_ = 2;
  ninner_nodes_ = ncontent_+1;
  parser_ = new cyclus::XMLParser();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void InfileTreeTest::TearDown() {
  delete parser_;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void InfileTreeTest::LoadParser() {
  std::stringstream ss("");
  GetContent(ss);
  //std::cout << ss.str() << std::endl;
  parser_->Init(ss);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(InfileTreeTest, constructor) {
  LoadParser();
  EXPECT_NO_THROW(cyclus::InfileTree engine(*parser_));
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(InfileTreeTest, top_level_queries) {
  LoadParser();
  cyclus::InfileTree engine(*parser_);
  EXPECT_EQ(engine.NElements(),ninner_nodes_);
  EXPECT_EQ(engine.NMatches(content_node_),ncontent_);
  EXPECT_EQ(engine.NMatches(inner_node_),1);
  EXPECT_EQ(engine.GetElementName(),content_node_);
  EXPECT_EQ(engine.GetString(content_node_),content_);
  for (int i = 0; i < ncontent_; i++) {
    EXPECT_EQ(engine.GetElementName(i),content_node_);
    EXPECT_EQ(engine.GetString(content_node_,i),content_);
  }
  EXPECT_EQ(engine.GetElementName(ncontent_),inner_node_);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(InfileTreeTest, top_level_throws) {
  LoadParser();
  cyclus::InfileTree engine(*parser_);
  EXPECT_THROW(engine.GetString(content_node_,ninner_nodes_+1), cyclus::ValueError);
  EXPECT_THROW(engine.GetString(inner_node_), cyclus::ValueError);
  EXPECT_THROW(engine.GetElementName(ninner_nodes_+1), cyclus::ValueError);
  EXPECT_THROW(engine.SubTree(content_node_,ninner_nodes_+1), cyclus::ValueError);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(InfileTreeTest, null_query) {
  LoadParser();
  cyclus::InfileTree engine(*parser_);
  std::string query = "something_silly";
  EXPECT_EQ(engine.NMatches(query), 0);
  EXPECT_THROW(engine.GetString(query), cyclus::KeyError);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(InfileTreeTest, mid_level_queries) {
  LoadParser();
  cyclus::InfileTree engine(*parser_);
  EXPECT_NO_THROW(cyclus::InfileTree* qe = engine.SubTree(inner_node_));
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(InfileTreeTest, low_level_queries) {
  LoadParser();
  cyclus::InfileTree engine(*parser_);
  cyclus::InfileTree* qe = engine.SubTree(inner_node_);
  EXPECT_EQ(qe->GetElementName(),unknown_node_);
  cyclus::InfileTree* qe2 = qe->SubTree(unknown_node_);
  EXPECT_EQ(qe2->GetString(content_node_),content_);
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(InfileTreeTest, optional_queries) {
  using std::string;
  using cyclus::OptionalQuery;

  std::stringstream ss;
  string other = "other";
  double dbl_val = 1.4;
  double dbl_other = 1.5;
  string dbl_str = "double";
  int int_val = 3;
  int int_other = 5;
  string int_str = "int";
  string str_val = "str";
  string str_other = "some_str";
  string str_str = "string";
  ss << "<root>"
     << "<" << dbl_str << ">" << dbl_val << "</" << dbl_str << ">"
     << "<" << int_str << ">" << int_val << "</" << int_str << ">"
     << "<" << str_str << ">" << str_val << "</" << str_str << ">"
     << "</root>";

  cyclus::XMLParser parser;
  parser.Init(ss);
  cyclus::InfileTree qe(parser);

  EXPECT_DOUBLE_EQ(dbl_val, OptionalQuery<double>(&qe, dbl_str, dbl_other));
  EXPECT_DOUBLE_EQ(dbl_other, OptionalQuery<double>(&qe, other, dbl_other));

  EXPECT_EQ(int_val, OptionalQuery<int>(&qe, int_str, int_other));
  EXPECT_EQ(int_other, OptionalQuery<int>(&qe, other, int_other));

  EXPECT_EQ(str_val, OptionalQuery<string>(&qe, str_str, str_other));
  EXPECT_EQ(str_other, OptionalQuery<string>(&qe, other, str_other));
}
