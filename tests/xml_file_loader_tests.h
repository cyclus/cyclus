#ifndef CYCLUS_TESTS_XML_FILE_LOADER_TESTS_H_
#define CYCLUS_TESTS_XML_FILE_LOADER_TESTS_H_

#include <fstream>
#include <iostream>
#include <string>
#include <unistd.h>

#include <gtest/gtest.h>

#include "recorder.h"
#include "xml_file_loader.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class XMLFileLoaderTests : public ::testing::Test {
 public:
  std::string control_file, decay_control_file, solver_control_file, eps_file,
      false_file, module_file, recipe_file, package_file;

  cyclus::FullBackend* b_;
  cyclus::Recorder rec_;
  std::string schema_path;

  virtual void SetUp();

  virtual void TearDown();

  std::string FalseSequence();
  std::string ControlSequence();
  std::string ControlSequenceWithDecay();
  std::string ControlSequenceWithSolver();
  std::string ControlSequenceWithEps();
  std::string RecipeSequence();
  std::string ModuleSequence();
  std::string ControlSchema();
  std::string PackageSequence();

 private:
  void CreateTestInputFile(std::string fname, std::string contents) {
    std::ofstream outFile(fname.c_str());
    outFile << contents;
    outFile.close();
  }
};

#endif  // CYCLUS_TESTS_XML_FILE_LOADER_TESTS_H_
