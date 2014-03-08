// query_engine_tests.cc
#include "query_engine_tests.h"

#include <iostream>
#include "xml_parser.h"
#include "error.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
std::string QueryEngineTest::Unknowncontent() {
  std::stringstream ss("");
  ss << "    <" << unknown_node_ << ">" << std::endl
     << "      <" << content_node_ << ">" << content_ << "</" << content_node_ << ">" << std::endl
     << "    </" << unknown_node_ << ">" << std::endl;
  return ss.str();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
std::string QueryEngineTest::Subcontent() {
  std::stringstream ss("");
  ss  << "  <" << content_node_ << ">" << content_ << "</" << content_node_ << ">" << std::endl;
  return ss.str();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void QueryEngineTest::GetContent(std::stringstream &ss) {  
  ss << "<" << root_node_ << ">" << std::endl;
  for (int i = 0; i < ncontent_; i++) {
    ss << Subcontent();
  }
  ss << "  <" << inner_node_ << ">"  << std::endl
     << Unknowncontent()
     << "  </" << inner_node_ << ">" << std::endl
     << "</" << root_node_ << ">";
}
 
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void QueryEngineTest::SetUp() {
  root_node_ = "facility";
  content_node_ = "name";
  content_ = "the_name";
  inner_node_ = "module";
  unknown_node_ = "unknown_module_name";
  ncontent_ = 2;
  ninner_nodes_ = ncontent_+1;
  parser_ = new cyclus::XMLParser();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void QueryEngineTest::TearDown() {
  delete parser_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void QueryEngineTest::LoadParser() {
  std::stringstream ss("");
  GetContent(ss);
  //std::cout << ss.str() << std::endl;
  parser_->Init(ss);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(QueryEngineTest, constructor) {  
  LoadParser();
  EXPECT_NO_THROW(cyclus::QueryEngine engine(*parser_));
} 

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(QueryEngineTest, top_level_queries) {  
  LoadParser();
  cyclus::QueryEngine engine(*parser_);
  EXPECT_EQ(engine.NElements(),ninner_nodes_);
  EXPECT_EQ(engine.NElementsMatchingQuery(content_node_),ncontent_);
  EXPECT_EQ(engine.NElementsMatchingQuery(inner_node_),1);
  EXPECT_EQ(engine.GetElementName(),content_node_);
  EXPECT_EQ(engine.GetString(content_node_),content_);
  for (int i = 0; i < ncontent_; i++) {
    EXPECT_EQ(engine.GetElementName(i),content_node_);
    EXPECT_EQ(engine.GetString(content_node_,i),content_);
  }
  EXPECT_EQ(engine.GetElementName(ncontent_),inner_node_);
} 

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(QueryEngineTest, top_level_throws) {  
  LoadParser();
  cyclus::QueryEngine engine(*parser_);
  EXPECT_THROW(engine.GetString(content_node_,ninner_nodes_+1), cyclus::ValueError);
  EXPECT_THROW(engine.GetString(inner_node_), cyclus::ValueError);
  EXPECT_THROW(engine.GetElementName(ninner_nodes_+1), cyclus::ValueError);
  EXPECT_THROW(engine.QueryElement(content_node_,ninner_nodes_+1), cyclus::ValueError);  
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(QueryEngineTest, null_query) {  
  LoadParser();
  cyclus::QueryEngine engine(*parser_);
  std::string query = "something_silly";
  EXPECT_EQ(engine.NElementsMatchingQuery(query), 0);
  EXPECT_THROW(engine.GetString(query), cyclus::KeyError);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(QueryEngineTest, mid_level_queries) {  
  LoadParser();
  cyclus::QueryEngine engine(*parser_);
  EXPECT_NO_THROW(cyclus::QueryEngine* qe = engine.QueryElement(inner_node_));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(QueryEngineTest, low_level_queries) {  
  LoadParser();
  cyclus::QueryEngine engine(*parser_);
  cyclus::QueryEngine* qe = engine.QueryElement(inner_node_);
  EXPECT_EQ(qe->GetElementName(),unknown_node_);
  cyclus::QueryEngine* qe2 = qe->QueryElement(unknown_node_);
  EXPECT_EQ(qe2->GetString(content_node_),content_);
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(QueryEngineTest, optional_queries) {
  using std::string;
  using cyclus::GetOptionalQuery;
  
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
  cyclus::QueryEngine qe(parser);

  EXPECT_DOUBLE_EQ(dbl_val, GetOptionalQuery<double>(&qe, dbl_str, dbl_other));
  EXPECT_DOUBLE_EQ(dbl_other, GetOptionalQuery<double>(&qe, other, dbl_other));
  
  EXPECT_EQ(int_val, GetOptionalQuery<int>(&qe, int_str, int_other));
  EXPECT_EQ(int_other, GetOptionalQuery<int>(&qe, other, int_other));
  
  EXPECT_EQ(str_val, GetOptionalQuery<string>(&qe, str_str, str_other));
  EXPECT_EQ(str_other, GetOptionalQuery<string>(&qe, other, str_other));
}
