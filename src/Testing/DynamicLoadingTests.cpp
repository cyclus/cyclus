#include <gtest/gtest.h>

#include "Env.h"
#include "Model.h"
#include "Prototype.h"
#include "DynamicModule.h"

//#include <cstdlib>
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include "boost/filesystem.hpp"

using namespace std;
namespace fs = boost::filesystem;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST(DynamicLoadingTests,LoadTestFacility) 
{
  Model::loadModule("Facility","TestFacility");
                  Model::unloadModules();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST(DynamicLoadingTests, FindNonStandardPath) {
  // set up
  string name = "otherfac";
  string lib_name = "lib" + name + DynamicModule::suffix();
  fs::path path = fs::path(Env::getInstallPath()) / fs::path("test_path") / fs::path(lib_name);

  // create file
  fs::create_directory(path.parent_path());
  ofstream f(path.string().c_str());
  f.close();

  // add path to env
  string cmd = "CYCLUS_MODULE_PATH="+path.parent_path().string();
  putenv((char*)cmd.c_str());

  // test
  DynamicModule mod = DynamicModule("Facility", name);
  EXPECT_EQ(path.string(), mod.path());

  fs::remove_all(path);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST(DynamicLoadingTests, LoadLibError) {
  DynamicModule mod = DynamicModule("Facility","not_a_fac");
  EXPECT_THROW(mod.initialize(), CycIOException);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST(DynamicLoadingTests,ConstructTestFacility) 
{
  EXPECT_NO_THROW(Model::loadModule("Facility","TestFacility");
                  Model* fac = Model::constructModel("TestFacility");
                  Model::deleteModel(fac);
                  Model::unloadModules(););
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
TEST(DynamicLoadingTests,cloneTestFacility) 
{
  EXPECT_NO_THROW(Model::loadModule("Facility","TestFacility");
                  Model* fac = Model::constructModel("TestFacility");
                  Prototype* clone = dynamic_cast<Prototype*>(fac)->clone();
                  Model::deleteModel(dynamic_cast<Model*>(clone));
                  Model::deleteModel(fac);
                  Model::unloadModules(););
}
