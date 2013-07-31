#include <gtest/gtest.h>

#include <stdlib.h>
#include <fstream>
#include <iostream>
#include "boost/filesystem.hpp"

#include "Env.h"
#include "Error.h"
#include "Model.h"
#include "Prototype.h"
#include "DynamicModule.h"

namespace fs = boost::filesystem;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(DynamicLoadingTests, LoadTestFacility) {
  using cyclus::Model;
  EXPECT_NO_THROW(Model::loadModule("Facility", "TestFacility"));
  EXPECT_NO_THROW(Model::unloadModules());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(DynamicLoadingTests, FindNonStandardPath) {
  using cyclus::DynamicModule;

  // set up
  std::string name = "otherfac";
  std::string lib_name = "lib" + name + DynamicModule::suffix();
  fs::path path = fs::path(getenv("HOME")) / fs::path(".tmp-cyclus-test") / fs::path(lib_name);

  // create file
  fs::create_directory(path.parent_path());
  std::ofstream f(path.string().c_str());
  f.close();

  // add path to env 
  std::string cmd = cyclus::Env::moduleEnvVarName() + '=' + \
                    path.parent_path().string();
  putenv((char*)cmd.c_str());

  // test
  DynamicModule mod = DynamicModule("Facility", name);
  EXPECT_EQ(path.string(), mod.path()); // note path calls (private) setPath()

  fs::remove_all(path);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(DynamicLoadingTests, LoadLibError) {
  using cyclus::DynamicModule;
  DynamicModule mod = DynamicModule("Facility", "not_a_fac");
  EXPECT_THROW(mod.initialize(), cyclus::IOError);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(DynamicLoadingTests, ConstructTestFacility) {
  using cyclus::Model;
  EXPECT_NO_THROW(Model::loadModule("Facility", "TestFacility");
                  Model* fac = Model::constructModel("TestFacility");
                  Model::deleteModel(fac);
                  Model::unloadModules(););
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(DynamicLoadingTests, cloneTestFacility) {
  using cyclus::Model;
  using cyclus::Prototype;
  EXPECT_NO_THROW(Model::loadModule("Facility", "TestFacility");
                  Model* fac = Model::constructModel("TestFacility");
                  Prototype* clone = dynamic_cast<Prototype*>(fac)->clone();
                  Model::deleteModel(dynamic_cast<Model*>(clone));
                  Model::deleteModel(fac);
                  Model::unloadModules(););
}
