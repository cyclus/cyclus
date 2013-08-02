// XMLQueryEngineTests.cpp
#include "XMLQueryEngineTests.h"

#include <iostream>
#include "XMLParser.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
std::string XMLQueryEngineTest::unknowncontent() {
  std::stringstream ss("");
  ss << "    <" << unknown_node_ << ">" << std::endl
     << "      <" << content_node_ << ">" << content_ << "</" << content_node_ << ">" << std::endl
     << "    </" << unknown_node_ << ">" << std::endl;
  return ss.str();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
std::string XMLQueryEngineTest::subcontent() {
  std::stringstream ss("");
  ss  << "  <" << content_node_ << ">" << content_ << "</" << content_node_ << ">" << std::endl;
  return ss.str();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void XMLQueryEngineTest::getContent(std::stringstream &ss) {  
  ss << "<" << root_node_ << ">" << std::endl;
  for (int i = 0; i < ncontent_; i++) {
    ss << subcontent();
  }
  ss << "  <" << inner_node_ << ">"  << std::endl
     << unknowncontent()
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
void XMLQueryEngineTest::loadParser() {
  std::stringstream ss("");
  getContent(ss);
  //cout << ss.str() << std::endl;
  parser_ = new cyclus::XMLParser();
  parser_->init(ss);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(XMLQueryEngineTest,constructor) {  
  loadParser();
  EXPECT_NO_THROW(cyclus::XMLQueryEngine engine(*parser_));
} 

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(XMLQueryEngineTest,top_level_queries) {  
  loadParser();
  cyclus::XMLQueryEngine engine(*parser_);
  EXPECT_EQ(engine.nElements(),ninner_nodes_);
  EXPECT_EQ(engine.nElementsMatchingQuery(content_node_),ncontent_);
  EXPECT_EQ(engine.nElementsMatchingQuery(inner_node_),1);
  EXPECT_EQ(engine.getElementName(),content_node_);
  EXPECT_EQ(engine.getElementContent(content_node_),content_);
  for (int i = 0; i < ncontent_; i++) {
    EXPECT_EQ(engine.getElementName(i),content_node_);
    EXPECT_EQ(engine.getElementContent(content_node_,i),content_);
  }
  EXPECT_EQ(engine.getElementName(ncontent_),inner_node_);
} 

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(XMLQueryEngineTest,top_level_throws) {  
  using cyclus::CycIndexException;
  using cyclus::CycRangeException;
  loadParser();
  cyclus::XMLQueryEngine engine(*parser_);
  EXPECT_THROW(engine.getElementContent(content_node_,ninner_nodes_+1),CycIndexException);
  EXPECT_THROW(engine.getElementContent(inner_node_),CycRangeException);
  EXPECT_THROW(engine.getElementName(ninner_nodes_+1),CycIndexException);
  EXPECT_THROW(engine.queryElement(content_node_,ninner_nodes_+1),CycIndexException);  
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(XMLQueryEngineTest,mid_level_queries) {  
  loadParser();
  cyclus::XMLQueryEngine engine(*parser_);
  EXPECT_NO_THROW(cyclus::QueryEngine* qe = engine.queryElement(inner_node_));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(XMLQueryEngineTest,low_level_queries) {  
  loadParser();
  cyclus::XMLQueryEngine engine(*parser_);
  cyclus::QueryEngine* qe = engine.queryElement(inner_node_);
  EXPECT_EQ(qe->getElementName(),unknown_node_);
  cyclus::QueryEngine* qe2 = qe->queryElement(unknown_node_);
  EXPECT_EQ(qe2->getElementContent(content_node_),content_);
}
