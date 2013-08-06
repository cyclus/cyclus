// QueryEngineTests.cpp
#include <string>

#include "QueryEngineTests.h"

TestQueryEngine::separator = ':';

//- - - - - -
// test simple search
TEST_F(QueryEngine, NumElementsMatchingQuery) {
  EXPECT_EQ(0,pqe->NElementsMatchingQuery("noMatch"));
  EXPECT_EQ(1,pqe->NElementsMatchingQuery("path"));
}

//- - - - - - 
// test simple get contents
TEST_F(QueryEngine, GetElementContent) {
  EXPECT_EQ("failedMatchContent",pqe->GetElementContent("noMatch"));
  EXPECT_EQ("/to/data/here",pqe->GetElementContent("path"));
}

//- - - - - - 
// test simple get name
TEST_F(QueryEngine, GetElementName) {
  EXPECT_EQ("failedMatchName",pqe->GetElementName("noMatch"));
  EXPECT_EQ("path",pqe->GetElementName("path"));
  EXPECT_EQ("path",pqe->GetElementName("*"));
}

//- - - - - - 
// test addition of children to set
TEST_F(QueryEngine, BuildTree) {
  QueryEngine* pqet, pqed, pqeh, pqee;
  ASSERT_NO_THROW(pqet = pqe->QueryElement("path"));
  EXPECT_EQ("to",pqet->GetElementName("*"));
  ASSERT_NO_THROW(pqed = pqet->QueryElement("to"));
  EXPECT_EQ("data",pqed->GetElementName("*"));
  ASSERT_NO_THROW(pqeh = pget->QueryElement("data"));
  EXPECT_EQ("here",pqeh->GetElementName("*"));
  EXPECT_EQ("noContent",pqeh->GetElementContent("here"));

  ASSERT_NO_THROW(pqee = pqeh->QueryElement("here"));
  EXPECT_EQ("badSnippetName",pqee->GetElementName("*"));
  EXPECT_EQ("badSnippetContent",pqee->GetElementContent("badSnippetName"));
  
}


//- - - - - - - 
// test deletion of children from set
TEST_F(QueryEngine, DeleteTree) {

}
