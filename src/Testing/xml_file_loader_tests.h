// xml_file_loader_tests.h
#include <gtest/gtest.h>

#include <string>
#include <iostream>
#include <fstream>
#include <unistd.h>

#include "xml_file_loader.h"



//- - - - - - - - 
class XMLFileLoaderTests : public ::testing::Test {

 private:

  void CreateTestInputFile(std::string fname, std::string contents) {
    std::ofstream outFile(fname.c_str());
    outFile<<contents;
    outFile.close();
  }

 public:
  std::string controlFile, falseFile, moduleFile, recipeFile;

  cyclus::XMLFileLoader xmlFile;

  virtual void SetUp();

  virtual void TearDown();

  std::string FalseSequence();
  std::string ControlSequence();
  std::string RecipeSequence();
  std::string ModuleSequence();
  std::string ControlSchema();

};


