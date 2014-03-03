// xml_file_loader_tests.h
#ifndef CYCLUS_TESTS_XML_FILE_LOADER_TESTS_H_
#define CYCLUS_TESTS_XML_FILE_LOADER_TESTS_H_

#include <gtest/gtest.h>

#include <fstream>
#include <iostream>
#include <string>
#include <unistd.h>

#include "recorder.h"
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

  cyclus::FullBackend* b_;
  std::string schema_path;

  virtual void SetUp();

  virtual void TearDown();

  std::string FalseSequence();
  std::string ControlSequence();
  std::string RecipeSequence();
  std::string ModuleSequence();
  std::string ControlSchema();

};

#endif  // CYCLUS_TESTS_XML_FILE_LOADER_TESTS_H_
