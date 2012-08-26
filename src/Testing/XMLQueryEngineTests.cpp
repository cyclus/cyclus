// XMLQueryEngineTests.cpp
#include <string>
#include "XMLQueryEngineTests.h"



//- - - - - - - - - - - - - - 
// TEST_F(XMLQueryEngineTest, Constructors) {

//   ASSERT_NO_THROW(xqe = new XMLQueryEngine());
//   EXPECT_NE(xqe, 0);
//   ASSERT_NO_THROW(delete xqe);

//   ASSERT_NO_THROW(xqe = new XMLQueryEngine(testSnippetA.snippet));
//   EXPECT_NE(xqe, 0);
//   ASSERT_NO_THROW(delete xqe);

//   // need to add test for the construtor based on an existing doc
// }

// search for single entries in a snippet with only 1 entry
TEST_F(XMLQueryEngineTest, SearchOneOfOne) {

  ASSERT_NO_THROW(xqeA = XMLQueryEngine(testSnippetA.snippet));
  // find entry that is there
  EXPECT_EQ(testSnippetA.numA,xqeA.numElementsMatchingQuery(testElementA.path.c_str()));
  // confirm tht we don't find entry that isn't there
  EXPECT_EQ(testSnippetA.numB,xqeA.numElementsMatchingQuery(testElementB.path.c_str()));

}

// search for single entries in a snippet with multiple entries
TEST_F(XMLQueryEngineTest, SearchOneOfMany) {

  // find each of the different entries
  ASSERT_NO_THROW(xqeA = XMLQueryEngine(testSnippetB.snippet));
  EXPECT_EQ(testSnippetB.numA,xqeA.numElementsMatchingQuery(testElementA.path.c_str()));
  EXPECT_EQ(testSnippetB.numB,xqeA.numElementsMatchingQuery(testElementB.path.c_str()));

  // find each of the different entries in different order
  ASSERT_NO_THROW(xqeA = XMLQueryEngine(testSnippetC.snippet));
  EXPECT_EQ(testSnippetC.numA,xqeA.numElementsMatchingQuery(testElementA.path.c_str()));
  EXPECT_EQ(testSnippetC.numB,xqeA.numElementsMatchingQuery(testElementB.path.c_str()));

}

// search for 1 or more entries in a snippet with multiple entries
TEST_F(XMLQueryEngineTest, SearchManyOfMany) {

  // find the correct number of entries of each type (A,A)
  ASSERT_NO_THROW(xqeA = XMLQueryEngine(testSnippetD.snippet));
  EXPECT_EQ(testSnippetD.numA,xqeA.numElementsMatchingQuery(testElementA.path.c_str()));
  EXPECT_EQ(testSnippetD.numB,xqeA.numElementsMatchingQuery(testElementB.path.c_str()));

  // find the correct number of entries of each type (A,A,B)
  ASSERT_NO_THROW(xqeA = XMLQueryEngine(testSnippetE.snippet));
  EXPECT_EQ(testSnippetE.numA,xqeA.numElementsMatchingQuery(testElementA.path.c_str()));
  EXPECT_EQ(testSnippetE.numB,xqeA.numElementsMatchingQuery(testElementB.path.c_str()));

  // find the correct number of entries of each type (A,A,B,B)
  ASSERT_NO_THROW(xqeA = XMLQueryEngine(testSnippetF.snippet));
  EXPECT_EQ(testSnippetF.numA,xqeA.numElementsMatchingQuery(testElementA.path.c_str()));
  EXPECT_EQ(testSnippetF.numB,xqeA.numElementsMatchingQuery(testElementB.path.c_str()));

  // find the correct number of entries of each type (A,B,A,B)
  ASSERT_NO_THROW(xqeA = XMLQueryEngine(testSnippetG.snippet));
  EXPECT_EQ(testSnippetG.numA,xqeA.numElementsMatchingQuery(testElementA.path.c_str()));
  EXPECT_EQ(testSnippetG.numB,xqeA.numElementsMatchingQuery(testElementB.path.c_str()));

  // find the correct number of entries of each type (A,B,AC,B)
  ASSERT_NO_THROW(xqeA = XMLQueryEngine(testSnippetH.snippet));
  EXPECT_EQ(testSnippetH.numA,xqeA.numElementsMatchingQuery(testElementA.path.c_str()));
  EXPECT_EQ(testSnippetH.numB,xqeA.numElementsMatchingQuery(testElementB.path.c_str()));

}

TEST_F(XMLQueryEngineTest, SearchDeepSame) {

  ASSERT_NO_THROW(xqeA = XMLQueryEngine(testSnippetI.snippet));
  EXPECT_EQ(1,xqeA.numElementsMatchingQuery(testElementAAin.path.c_str()));
  EXPECT_EQ(1,xqeA.numElementsMatchingQuery(testElementAAout.path.c_str()));

}

TEST_F(XMLQueryEngineTest, SearchDeepDiff) {

  ASSERT_NO_THROW(xqeA = XMLQueryEngine(testSnippetJ.snippet));
  EXPECT_EQ(1,xqeA.numElementsMatchingQuery(testElementABin.path.c_str()));
  EXPECT_EQ(1,xqeA.numElementsMatchingQuery(testElementABout.path.c_str()));

}

// extract contents of a single entry in a snippet with only 1 entry
TEST_F(XMLQueryEngineTest, ExtractOneOfOne) {

  ASSERT_NO_THROW(xqeA = XMLQueryEngine(testSnippetA.snippet));
  EXPECT_EQ(testSnippetA.numA,xqeA.numElementsMatchingQuery(testElementA.path.c_str()));
  EXPECT_EQ(testElementA.content,xqeA.get_content());

}

// extract contents of a single entry in a snippet with only 1 entry
// USING the expression directly
TEST_F(XMLQueryEngineTest, ExtractOneOfOneByName) {

  ASSERT_NO_THROW(xqeA = XMLQueryEngine(testSnippetA.snippet));
  EXPECT_EQ(testElementA.content,xqeA.get_content(testElementA.path.c_str()));


}


// extract contents of a single entry in a snippet with multiple entries
TEST_F(XMLQueryEngineTest, ExtractOneOfMany) {

  int elenum; 

  ASSERT_NO_THROW(xqeA = XMLQueryEngine(testSnippetB.snippet));
  elenum=0;
  xqeA.numElementsMatchingQuery(testElementA.path.c_str());
  EXPECT_EQ(testElementA.content,xqeA.get_content(elenum));
  xqeA.numElementsMatchingQuery(testElementB.path.c_str());
  EXPECT_EQ(testElementB.content,xqeA.get_content(elenum));

  ASSERT_NO_THROW(xqeA = XMLQueryEngine(testSnippetC.snippet));
  elenum=0;
  xqeA.numElementsMatchingQuery(testElementA.path.c_str());
  EXPECT_EQ(testElementA.content,xqeA.get_content(elenum));
  xqeA.numElementsMatchingQuery(testElementB.path.c_str());
  EXPECT_EQ(testElementB.content,xqeA.get_content(elenum));
}

TEST_F(XMLQueryEngineTest, ExtractOneOfManyByName) {
  ASSERT_NO_THROW(xqeA = XMLQueryEngine(testSnippetB.snippet));
  EXPECT_EQ(testElementA.content,xqeA.get_content(testElementA.path.c_str()));
  EXPECT_EQ(testElementB.content,xqeA.get_content(testElementB.path.c_str()));
}

TEST_F(XMLQueryEngineTest, ExtractManyOfMany) {

  int elenum;

  ASSERT_NO_THROW(xqeA = XMLQueryEngine(testSnippetD.snippet));
  EXPECT_EQ(testSnippetD.numA,xqeA.numElementsMatchingQuery(testElementA.path.c_str()));
  elenum=0;
  EXPECT_EQ(testElementA.content,xqeA.get_content(elenum++));
  EXPECT_EQ(testElementA.content,xqeA.get_content(elenum++));

  ASSERT_NO_THROW(xqeA = XMLQueryEngine(testSnippetE.snippet));
  EXPECT_EQ(testSnippetE.numA,xqeA.numElementsMatchingQuery(testElementA.path.c_str()));
  elenum=0;
  EXPECT_EQ(testElementA.content,xqeA.get_content(elenum++));
  EXPECT_EQ(testElementA.content,xqeA.get_content(elenum++));

  ASSERT_NO_THROW(xqeA = XMLQueryEngine(testSnippetF.snippet));
  EXPECT_EQ(testSnippetF.numA,xqeA.numElementsMatchingQuery(testElementA.path.c_str()));
  elenum=0;
  EXPECT_EQ(testElementA.content,xqeA.get_content(elenum++));
  EXPECT_EQ(testElementA.content,xqeA.get_content(elenum++));

  ASSERT_NO_THROW(xqeA = XMLQueryEngine(testSnippetG.snippet));
  EXPECT_EQ(testSnippetG.numA,xqeA.numElementsMatchingQuery(testElementA.path.c_str()));
  elenum=0;
  EXPECT_EQ(testElementA.content,xqeA.get_content(elenum++));
  EXPECT_EQ(testElementA.content,xqeA.get_content(elenum++));

  ASSERT_NO_THROW(xqeA = XMLQueryEngine(testSnippetH.snippet));
  EXPECT_EQ(testSnippetH.numA,xqeA.numElementsMatchingQuery(testElementA.path.c_str()));
  elenum=0;
  EXPECT_EQ(testElementA.content,xqeA.get_content(elenum++));
  EXPECT_EQ(testElementAC.content,xqeA.get_content(elenum++));

}

TEST_F(XMLQueryEngineTest, ExtractChild) {

  ASSERT_NO_THROW(xqeA = XMLQueryEngine(testSnippetZ.snippet));
  EXPECT_EQ(1,xqeA.numElementsMatchingQuery("/start"));
  EXPECT_EQ(testElementA.element,xqeA.get_child(0,0));
  EXPECT_EQ(startContent,xqeA.get_child(0,1));

}


TEST_F(XMLQueryEngineTest, GetName) {

  int elenum;

  ASSERT_NO_THROW(xqeA = XMLQueryEngine(testSnippetG.snippet));
  std::string query = "/start*";
  EXPECT_EQ(xqeA.numElementsMatchingQuery(query),testSnippetG.numA+testSnippetG.numB);
  elenum = 0;
  EXPECT_EQ(xqeA.getElementName(query,elenum++),testElementA.name);
  EXPECT_EQ(xqeA.getElementName(query,elenum++),testElementB.name);
  EXPECT_EQ(xqeA.getElementName(query,elenum++),testElementA.name);
  EXPECT_EQ(xqeA.getElementName(query,elenum++),testElementB.name);
}
