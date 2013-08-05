// XMLQueryEngineTests.cpp
#include "XMLQueryEngineTests.h"

#include <iostream>
#include "XMLParser.h"
#include "error.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
std::string XMLQueryEngineTest::Unknowncontent() {
  std::stringstream ss("");
  ss << "    <" << unknown_node_ << ">" << std::endl
     << "      <" << content_node_ << ">" << content_ << "</" << content_node_ << ">" << std::endl
     << "    </" << unknown_node_ << ">" << std::endl;
  return ss.str();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
std::string XMLQueryEngineTest::Subcontent() {
  std::stringstream ss("");
  ss  << "  <" << content_node_ << ">" << content_ << "</" << content_node_ << ">" << std::endl;
  return ss.str();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void XMLQueryEngineTest::GetContent(std::stringstream &ss) {  
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
void XMLQueryEngineTest::SetUp() {
  root_node_ = "facility";
  content_node_ = "name";
  content_ = "the_name";
  inner_node_ = "module";
  unknown_node_ = "unknown_module_name";
  ncontent_ = 2;
  ninner_nodes_ = ncontent_+1;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void XMLQueryEngineTest::TearDown() {
  if (parser_)
    delete parser_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void XMLQueryEngineTest::LoadParser() {
  std::stringstream ss("");
  GetContent(ss);
  //cout << ss.str() << std::endl;
  parser_ = new cyclus::XMLParser();
  parser_->Init(ss);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(XMLQueryEngineTest,constructor) {  
  LoadParser();
  EXPECT_NO_THROW(cyclus::XMLQueryEngine engine(*parser_));
} 

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(XMLQueryEngineTest,top_level_queries) {  
  LoadParser();
  cyclus::XMLQueryEngine engine(*parser_);
  EXPECT_EQ(engine.NElements(),ninner_nodes_);
  EXPECT_EQ(engine.NElementsMatchingQuery(content_node_),ncontent_);
  EXPECT_EQ(engine.NElementsMatchingQuery(inner_node_),1);
  EXPECT_EQ(engine.GetElementName(),content_node_);
  EXPECT_EQ(engine.GetElementContent(content_node_),content_);
  for (int i = 0; i < ncontent_; i++) {
    EXPECT_EQ(engine.GetElementName(i),content_node_);
    EXPECT_EQ(engine.GetElementContent(content_node_,i),content_);
  }
  EXPECT_EQ(engine.GetElementName(ncontent_),inner_node_);
} 

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(XMLQueryEngineTest,top_level_throws) {  
  LoadParser();
  cyclus::XMLQueryEngine engine(*parser_);
  EXPECT_THROW(engine.GetElementContent(content_node_,ninner_nodes_+1), cyclus::ValueError);
  EXPECT_THROW(engine.GetElementContent(inner_node_), cyclus::ValueError);
  EXPECT_THROW(engine.GetElementName(ninner_nodes_+1), cyclus::ValueError);
  EXPECT_THROW(engine.QueryElement(content_node_,ninner_nodes_+1), cyclus::ValueError);  
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(XMLQueryEngineTest,mid_level_queries) {  
  LoadParser();
  cyclus::XMLQueryEngine engine(*parser_);
  EXPECT_NO_THROW(cyclus::QueryEngine* qe = engine.QueryElement(inner_node_));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(XMLQueryEngineTest,low_level_queries) {  
  LoadParser();
  cyclus::XMLQueryEngine engine(*parser_);
  cyclus::QueryEngine* qe = engine.QueryElement(inner_node_);
  EXPECT_EQ(qe->GetElementName(),unknown_node_);
  cyclus::QueryEngine* qe2 = qe->QueryElement(unknown_node_);
  EXPECT_EQ(qe2->GetElementContent(content_node_),content_);
}
