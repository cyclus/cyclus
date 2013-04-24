// XMLQueryEngineTests.cpp
#include "XMLQueryEngineTests.h"

#include <iostream>
#include "XMLParser.h"

using namespace std;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
std::string XMLQueryEngineTest::unknowncontent() {
  stringstream ss("");
  ss << "    <" << unknown_node_ << ">" << endl
     << "      <" << content_node_ << ">" << content_ << "</" << content_node_ << ">" << endl
     << "    </" << unknown_node_ << ">" << endl;
  return ss.str();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
std::string XMLQueryEngineTest::subcontent() {
  stringstream ss("");
  ss  << "  <" << content_node_ << ">" << content_ << "</" << content_node_ << ">" << endl;
  return ss.str();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void XMLQueryEngineTest::getContent(std::stringstream &ss) {  
  ss << "<" << root_node_ << ">" << endl;
  for (int i = 0; i < ncontent_; i++) {
    ss << subcontent();
  }
  ss << "  <" << inner_node_ << ">"  << endl
     << unknowncontent()
     << "  </" << inner_node_ << ">" << endl
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
  stringstream ss("");
  getContent(ss);
  //cout << ss.str() << endl;
  parser_ = new XMLParser();
  parser_->init(ss);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(XMLQueryEngineTest,constructor) {  
  loadParser();
  EXPECT_NO_THROW(XMLQueryEngine engine(*parser_));
} 

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(XMLQueryEngineTest,top_level_queries) {  
  loadParser();
  XMLQueryEngine engine(*parser_);
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
  loadParser();
  XMLQueryEngine engine(*parser_);
  EXPECT_THROW(engine.getElementContent(content_node_,ninner_nodes_+1),CycIndexException);
  EXPECT_THROW(engine.getElementContent(inner_node_),CycRangeException);
  EXPECT_THROW(engine.getElementName(ninner_nodes_+1),CycIndexException);
  EXPECT_THROW(engine.queryElement(content_node_,ninner_nodes_+1),CycIndexException);  
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(XMLQueryEngineTest,mid_level_queries) {  
  loadParser();
  XMLQueryEngine engine(*parser_);
  EXPECT_NO_THROW(QueryEngine* qe = engine.queryElement(inner_node_));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(XMLQueryEngineTest,low_level_queries) {  
  loadParser();
  XMLQueryEngine engine(*parser_);
  QueryEngine* qe = engine.queryElement(inner_node_);
  EXPECT_EQ(qe->getElementName(),unknown_node_);
  QueryEngine* qe2 = qe->queryElement(unknown_node_);
  EXPECT_EQ(qe2->getElementContent(content_node_),content_);
}
