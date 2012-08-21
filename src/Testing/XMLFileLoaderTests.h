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
  static std::string testFile1Contents;

 public:
  std::string testFile1;
  XMLFileLoader xmlFile;

  virtual void SetUp() {
    testFile1 = "test1.xml";
    std::ofstream outFile;
    outFile.open(testFile1.c_str());
    outFile << testFile1Contents;
    outFile.close();
  };
  virtual void TearDown() {
    // unlink(testFile1.c_str());

  };


};


