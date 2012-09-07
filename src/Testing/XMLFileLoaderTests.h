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
  static std::string controlSequence, falseSequence, moduleSequence, recipeSequence;
  std::string controlFile, falseFile, moduleFile, recipeFile;

  XMLFileLoader* xmlFile;

  virtual void SetUp();

  virtual void TearDown();

  void testModuleOpening();

};


