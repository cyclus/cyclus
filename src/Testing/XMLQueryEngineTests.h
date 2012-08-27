// XMLQueryEngineTests.h
#include <gtest/gtest.h>

#include <string>

#include "XMLQueryEngine.h"

//- - - - - - - - - - - - - - - - - 
class XMLQueryEngineTest : public ::testing::Test {

 protected:

  XMLQueryEngine *xqe;

  std::string startContent;

  struct TestElement {
    std::string name;
    std::string content;
    std::string element;
    std::string path;

    TestElement() {
      name = "";
      content = "";
      element = "";
      path = "";
    };

    TestElement(std::string nm, std::string cntnt) { 
      name=nm; 
      content=cntnt; 
      element="<" + name + ">" + content + "</" + name + ">";
      path=name;
    }
  } testElementA, testElementB, testElementAC, 
    testElementAAin, testElementAAout,
    testElementABin, testElementABout;

  struct TestSnippet {
    std::string snippet;
    int numA, numB;
    
  TestSnippet(std::string s="", int nA=0, int nB=0) : 
    snippet(s), numA(nA), numB(nB) {};
    
  } testSnippetA, testSnippetB, testSnippetC, testSnippetD, 
    testSnippetE, testSnippetF, testSnippetG, testSnippetH,
    testSnippetI, testSnippetJ,
    testSnippetZ;


  virtual void SetUp() {
    
    testElementA =  TestElement("elementA","contentA");
    testElementB =  TestElement("elementB","contentB");
    testElementAC = TestElement("elementA","contentC");

    int numA, numB;

    numA = 1, numB = 0;
    testSnippetA = TestSnippet(testElementA.element,numA,numB);

    numA = 1, numB = 1;
    testSnippetB = TestSnippet(testElementA.element+testElementB.element,numA,numB);

    numA = 1, numB = 1;
    testSnippetC = TestSnippet(testElementB.element+testElementA.element,numA,numB);

    numA = 2, numB = 0;
    testSnippetD = TestSnippet(testElementA.element+testElementA.element,numA,numB);

    numA = 2, numB = 1;
    testSnippetE = TestSnippet(testElementA.element+testElementA.element+testElementB.element,numA,numB);

    numA = 2, numB = 2;
    testSnippetF = TestSnippet(testElementA.element+testElementA.element+testElementB.element+testElementB.element,numA,numB);

    numA = 2, numB = 2;
    testSnippetG = TestSnippet(testElementA.element+testElementB.element+testElementA.element+testElementB.element,numA,numB);

    numA = 2, numB = 2;
    testSnippetH = TestSnippet(testElementA.element+testElementB.element+testElementAC.element+testElementB.element,numA,numB);

    testElementAAin  = testElementA;
    testElementAAout = TestElement("elementA",testElementAAin.element);
    testElementAAin.path = testElementAAout.path + "/" + testElementAAin.name;
    testSnippetI = TestSnippet(testElementAAout.element,numA,numB);

    testElementABin  = testElementB;
    testElementABout = TestElement("elementA",testElementABin.element);
    testElementABin.path = testElementABout.path + "/" + testElementABin.name;
    testSnippetJ = TestSnippet(testElementABout.element,numA,numB);

    numA = 1, numB = 0;
    startContent = "startContent";
    testSnippetZ = TestSnippet(testElementA.element+startContent,numA,numB);
  }

  virtual void TearDown() {

  }

};
