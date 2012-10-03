// QueryEngineTests.cpp
#include <string>

#include "QueryEngineTests.h"

TestQueryEngine::separator = ':';

//- - - - - -
// test simple search
TEST_F(QueryEngine, NumElementsMatchingQuery) {
  EXPECT_EQ(0,pqe->nElementsMatchingQuery("noMatch"));
  EXPECT_EQ(1,pqe->nElementsMatchingQuery("path"));
}

//- - - - - - 
// test simple get contents
TEST_F(QueryEngine, GetElementContent) {
  EXPECT_EQ("failedMatchContent",pqe->getElementContent("noMatch"));
  EXPECT_EQ("/to/data/here",pqe->getElementContent("path"));
}

//- - - - - - 
// test simple get name
TEST_F(QueryEngine, GetElementName) {
  EXPECT_EQ("failedMatchName",pqe->getElementName("noMatch"));
  EXPECT_EQ("path",pqe->getElementName("path"));
  EXPECT_EQ("path",pqe->getElementName("*"));
}

//- - - - - - 
// test addition of children to set
TEST_F(QueryEngine, BuildTree) {
  QueryEngine* pqet, pqed, pqeh, pqee;
  ASSERT_NO_THROW(pqet = pqe->queryElement("path"));
  EXPECT_EQ("to",pqet->getElementName("*"));
  ASSERT_NO_THROW(pqed = pqet->queryElement("to"));
  EXPECT_EQ("data",pqed->getElementName("*"));
  ASSERT_NO_THROW(pqeh = pget->queryElement("data"));
  EXPECT_EQ("here",pqeh->getElementName("*"));
  EXPECT_EQ("noContent",pqeh->getElementContent("here"));

  ASSERT_NO_THROW(pqee = pqeh->queryElement("here"));
  EXPECT_EQ("badSnippetName",pqee->getElementName("*"));
  EXPECT_EQ("badSnippetContent",pqee->getElementContent("badSnippetName"));
  
}


//- - - - - - - 
// test deletion of children from set
TEST_F(QueryEngine, DeleteTree) {

}
