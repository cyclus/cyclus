// XMLFileLoaderTests.h
#include <gtest/gtest.h>

#include <string>
#include <iostream>
#include <fstream>
#include <unistd.h>

#include "XMLFileLoader.h"



//- - - - - - - - 
class XMLFileLoaderTests : public ::testing::Test {

 private:
  static std::string testFile1Contents, testFile2Contents, testFile3Contents;

  void createTestInputFile(std::string fname, std::string contents) {
    std::ofstream outFile(fname.c_str());
    outFile<<contents;
    outFile.close();
  }

 public:
  std::string testFile1, testFile2, testFile3;
  XMLFileLoader xmlFile;

  virtual void SetUp() {
    testFile1 = "simpleXMLFile.xml";
    createTestInputFile(testFile1,testFile1Contents);

    testFile2 = "notAnXMLFile.txt";
    createTestInputFile(testFile2,testFile2Contents);

    testFile3 = "validCyclusTestInput.xml";
    createTestInputFile(testFile3,testFile3Contents);
  };

  virtual void TearDown() {
    unlink(testFile1.c_str());
    unlink(testFile2.c_str());
    unlink(testFile3.c_str());
  };


};


