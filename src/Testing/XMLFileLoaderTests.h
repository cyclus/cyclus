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

  void createTestInputFile(std::string fname, std::string contents) {
    std::ofstream outFile(fname.c_str());
    outFile<<contents;
    outFile.close();
  }

 public:
  std::string controlFile, falseFile, moduleFile, recipeFile;

  XMLFileLoader xmlFile;

  virtual void SetUp();

  virtual void TearDown();

  std::string falseSequence();
  std::string controlSequence();
  std::string recipeSequence();
  std::string moduleSequence();
  std::string controlSchema();

};


