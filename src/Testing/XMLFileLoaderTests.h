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

  virtual void SetUp() {
    falseFile = "false.xml";
    createTestInputFile(falseFile,falseSequence);

    controlFile = "control.xml";
    createTestInputFile(controlFile,controlSequence);

    recipeFile = "recipes.xml";
    createTestInputFile(recipeFile,recipeSequence);

    moduleFile = "modules.xml";
    createTestInputFile(moduleFile,moduleSequence);
  };

  virtual void TearDown() {
    unlink(falseFile.c_str());
    unlink(controlFile.c_str());
    unlink(recipeFile.c_str());
    unlink(moduleFile.c_str());
  };


};


