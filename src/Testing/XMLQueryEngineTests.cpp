// XMLQueryEngineTests.cpp
#include <string>
#include "XMLQueryEngineTests.h"
#include <iostream>
#include <sstream>

using namespace std;

// search for single entries in a snippet with only 1 entry
TEST_F(XMLQueryEngineTest, SearchOneOfOne) {

  cout << testSnippetA.snippet << endl;
  ASSERT_NO_THROW(xqe = new XMLQueryEngine(testSnippetA.snippet));
  // find entry that is there
  string query = "/"+testElementA.name;
  cout << xqe->getElementName() << endl;
  cout << query << endl;
  cout << xqe->numElementsMatchingQuery(query) << endl;
  //EXPECT_EQ(testSnippetA.numA,xqe->numElementsMatchingQuery(query.c_str()));
  // confirm tht we don't find entry that isn't there
  //EXPECT_EQ(testSnippetA.numB,xqe->numElementsMatchingQuery(testElementB.path.c_str()));

  delete xqe;
}

// // search for single entries in a snippet with multiple entries
// TEST_F(XMLQueryEngineTest, SearchOneOfMany) {

//   // find each of the different entries
//   ASSERT_NO_THROW(xqe = new XMLQueryEngine(testSnippetB.snippet));
//   EXPECT_EQ(testSnippetB.numA,xqe->numElementsMatchingQuery(testElementA.path.c_str()));
//   EXPECT_EQ(testSnippetB.numB,xqe->numElementsMatchingQuery(testElementB.path.c_str()));

//   delete xqe;

//   // find each of the different entries in different order
//   ASSERT_NO_THROW(xqe = new XMLQueryEngine(testSnippetC.snippet));
//   EXPECT_EQ(testSnippetC.numA,xqe->numElementsMatchingQuery(testElementA.path.c_str()));
//   EXPECT_EQ(testSnippetC.numB,xqe->numElementsMatchingQuery(testElementB.path.c_str()));

//   delete xqe;
// }

// // search for 1 or more entries in a snippet with multiple entries
// TEST_F(XMLQueryEngineTest, SearchManyOfMany) {

//   // find the correct number of entries of each type (A,A)
//   ASSERT_NO_THROW(xqe = new XMLQueryEngine(testSnippetD.snippet));
//   EXPECT_EQ(testSnippetD.numA,xqe->numElementsMatchingQuery(testElementA.path.c_str()));
//   EXPECT_EQ(testSnippetD.numB,xqe->numElementsMatchingQuery(testElementB.path.c_str()));

//   delete xqe;

//   // find the correct number of entries of each type (A,A,B)
//   ASSERT_NO_THROW(xqe = new XMLQueryEngine(testSnippetE.snippet));
//   EXPECT_EQ(testSnippetE.numA,xqe->numElementsMatchingQuery(testElementA.path.c_str()));
//   EXPECT_EQ(testSnippetE.numB,xqe->numElementsMatchingQuery(testElementB.path.c_str()));

//   delete xqe;

//   // find the correct number of entries of each type (A,A,B,B)
//   ASSERT_NO_THROW(xqe = new XMLQueryEngine(testSnippetF.snippet));
//   EXPECT_EQ(testSnippetF.numA,xqe->numElementsMatchingQuery(testElementA.path.c_str()));
//   EXPECT_EQ(testSnippetF.numB,xqe->numElementsMatchingQuery(testElementB.path.c_str()));

//   delete xqe;

//   // find the correct number of entries of each type (A,B,A,B)
//   ASSERT_NO_THROW(xqe = new XMLQueryEngine(testSnippetG.snippet));
//   EXPECT_EQ(testSnippetG.numA,xqe->numElementsMatchingQuery(testElementA.path.c_str()));
//   EXPECT_EQ(testSnippetG.numB,xqe->numElementsMatchingQuery(testElementB.path.c_str()));

//   delete xqe;

//   // find the correct number of entries of each type (A,B,AC,B)
//   ASSERT_NO_THROW(xqe = new XMLQueryEngine(testSnippetH.snippet));
//   EXPECT_EQ(testSnippetH.numA,xqe->numElementsMatchingQuery(testElementA.path.c_str()));
//   EXPECT_EQ(testSnippetH.numB,xqe->numElementsMatchingQuery(testElementB.path.c_str()));

//   delete xqe;
// }

// TEST_F(XMLQueryEngineTest, SearchDeepSame) {

//   ASSERT_NO_THROW(xqe = new XMLQueryEngine(testSnippetI.snippet));
//   EXPECT_EQ(1,xqe->numElementsMatchingQuery(testElementAAin.path.c_str()));
//   EXPECT_EQ(1,xqe->numElementsMatchingQuery(testElementAAout.path.c_str()));

//   delete xqe;
// }

// TEST_F(XMLQueryEngineTest, SearchDeepDiff) {

//   ASSERT_NO_THROW(xqe = new XMLQueryEngine(testSnippetJ.snippet));
//   EXPECT_EQ(1,xqe->numElementsMatchingQuery(testElementABin.path.c_str()));
//   EXPECT_EQ(1,xqe->numElementsMatchingQuery(testElementABout.path.c_str()));

//   delete xqe;
// }

// // extract contents of a single entry in a snippet with only 1 entry
// TEST_F(XMLQueryEngineTest, ExtractOneOfOne) {

//   ASSERT_NO_THROW(xqe = new XMLQueryEngine(testSnippetA.snippet));
//   EXPECT_EQ(testSnippetA.numA,xqe->numElementsMatchingQuery(testElementA.path.c_str()));
//   EXPECT_EQ(testElementA.content,xqe->get_content());

//   delete xqe;
// }

// // extract contents of a single entry in a snippet with only 1 entry
// // USING the expression directly
// TEST_F(XMLQueryEngineTest, ExtractOneOfOneByName) {

//   ASSERT_NO_THROW(xqe = new XMLQueryEngine(testSnippetA.snippet));
//   EXPECT_EQ(testElementA.content,xqe->get_content(testElementA.path.c_str()));

//   delete xqe;
// }


// // extract contents of a single entry in a snippet with multiple entries
// TEST_F(XMLQueryEngineTest, ExtractOneOfMany) {

//   int elenum; 

//   ASSERT_NO_THROW(xqe = new XMLQueryEngine(testSnippetB.snippet));
//   elenum=0;
//   xqe->numElementsMatchingQuery(testElementA.path.c_str());
//   EXPECT_EQ(testElementA.content,xqe->get_content(elenum));
//   xqe->numElementsMatchingQuery(testElementB.path.c_str());
//   EXPECT_EQ(testElementB.content,xqe->get_content(elenum));

//   ASSERT_NO_THROW(xqe = new XMLQueryEngine(testSnippetC.snippet));
//   elenum=0;
//   xqe->numElementsMatchingQuery(testElementA.path.c_str());
//   EXPECT_EQ(testElementA.content,xqe->get_content(elenum));
//   xqe->numElementsMatchingQuery(testElementB.path.c_str());
//   EXPECT_EQ(testElementB.content,xqe->get_content(elenum));

//   delete xqe;
// }

// TEST_F(XMLQueryEngineTest, ExtractOneOfManyByName) {
//   ASSERT_NO_THROW(xqe = new XMLQueryEngine(testSnippetB.snippet));
//   EXPECT_EQ(testElementA.content,xqe->get_content(testElementA.path.c_str()));
//   EXPECT_EQ(testElementB.content,xqe->get_content(testElementB.path.c_str()));

//   delete xqe;
// }

// TEST_F(XMLQueryEngineTest, ExtractManyOfMany) {

//   int elenum;

//   ASSERT_NO_THROW(xqe = new XMLQueryEngine(testSnippetD.snippet));
//   EXPECT_EQ(testSnippetD.numA,xqe->numElementsMatchingQuery(testElementA.path.c_str()));
//   elenum=0;
//   EXPECT_EQ(testElementA.content,xqe->get_content(elenum++));
//   EXPECT_EQ(testElementA.content,xqe->get_content(elenum++));

//   delete xqe;

//   ASSERT_NO_THROW(xqe = new XMLQueryEngine(testSnippetE.snippet));
//   EXPECT_EQ(testSnippetE.numA,xqe->numElementsMatchingQuery(testElementA.path.c_str()));
//   elenum=0;
//   EXPECT_EQ(testElementA.content,xqe->get_content(elenum++));
//   EXPECT_EQ(testElementA.content,xqe->get_content(elenum++));

//   delete xqe;

//   ASSERT_NO_THROW(xqe = new XMLQueryEngine(testSnippetF.snippet));
//   EXPECT_EQ(testSnippetF.numA,xqe->numElementsMatchingQuery(testElementA.path.c_str()));
//   elenum=0;
//   EXPECT_EQ(testElementA.content,xqe->get_content(elenum++));
//   EXPECT_EQ(testElementA.content,xqe->get_content(elenum++));

//   delete xqe;

//   ASSERT_NO_THROW(xqe = new XMLQueryEngine(testSnippetG.snippet));
//   EXPECT_EQ(testSnippetG.numA,xqe->numElementsMatchingQuery(testElementA.path.c_str()));
//   elenum=0;
//   EXPECT_EQ(testElementA.content,xqe->get_content(elenum++));
//   EXPECT_EQ(testElementA.content,xqe->get_content(elenum++));

//   delete xqe;

//   ASSERT_NO_THROW(xqe = new XMLQueryEngine(testSnippetH.snippet));
//   EXPECT_EQ(testSnippetH.numA,xqe->numElementsMatchingQuery(testElementA.path.c_str()));
//   elenum=0;
//   EXPECT_EQ(testElementA.content,xqe->get_content(elenum++));
//   EXPECT_EQ(testElementAC.content,xqe->get_content(elenum++));

//   delete xqe;
// }

// TEST_F(XMLQueryEngineTest, ExtractChild) {

//   ASSERT_NO_THROW(xqe = new XMLQueryEngine(testSnippetZ.snippet));
//   EXPECT_EQ(1,xqe->numElementsMatchingQuery("/start"));
//   EXPECT_EQ(testElementA.element,xqe->get_child(0,0));
//   EXPECT_EQ(startContent,xqe->get_child(0,1));

//   delete xqe;
// }

// TEST_F(XMLQueryEngineTest, GetName) {

//   int elenum;

//   ASSERT_NO_THROW(xqe = new XMLQueryEngine(testSnippetG.snippet));
//   string query = "/start/*";
//   EXPECT_EQ(xqe->numElementsMatchingQuery(query),testSnippetG.numA+testSnippetG.numB);
//   elenum = 0;
//   EXPECT_EQ(xqe->getElementName(elenum++),testElementA.name);
//   EXPECT_EQ(xqe->getElementName(elenum++),testElementB.name);
//   EXPECT_EQ(xqe->getElementName(elenum++),testElementA.name);
//   EXPECT_EQ(xqe->getElementName(elenum++),testElementB.name);

//   delete xqe;
// }

// TEST_F(XMLQueryEngineTest,TestNesting) {
//   stringstream ss("");
//   string query = "name";
//   string content = "region";
//   string subcontent = "module";

//   ss << "<" << content << ">\n"
//      << "  <" << query << ">" << content << "</" << query << ">\n"
//      << "  <" << subcontent << ">\n"
//      << "    <" << query << ">" << subcontent << "</" << query << ">\n"
//      << "  </" << subcontent << ">\n"
//      << "</" << content << ">";

//   cout << ss.str() << endl;

//   xqe = new XMLQueryEngine(ss.str());
//   string element_name = xqe->getElementName();
//   cout << element_name << endl;
//   //QueryEngine* qe = xqe->queryElement(element_name);

//   //EXPECT_EQ(content,qe->getElementContent("/name"));
//   //QueryEngine* subqe = xqe->queryElement(subcontent);
//   //EXPECT_EQ(subcontent,subqe->getElementContent("name"));
// }
