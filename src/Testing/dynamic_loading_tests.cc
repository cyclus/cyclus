#include <gtest/gtest.h>

#include <stdlib.h>
#include <fstream>
#include <iostream>
#include "boost/filesystem.hpp"

#include "context.h"
#include "env.h"
#include "error.h"
#include "event_manager.h"
#include "model.h"
#include "prototype.h"
#include "timer.h"
#include "dynamic_module.h"

namespace fs = boost::filesystem;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(DynamicLoadingTests, LoadTestFacility) {
  using cyclus::Model;
  EXPECT_NO_THROW(Model::LoadModule("Facility", "TestFacility"));
  EXPECT_NO_THROW(Model::UnloadModules());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(DynamicLoadingTests, FindNonStandardPath) {
  using cyclus::DynamicModule;

  // set up
  std::string name = "otherfac";
  std::string lib_name = "lib" + name + DynamicModule::Suffix();
  fs::path path = fs::path(getenv("HOME")) / fs::path(".tmp-cyclus-test") / fs::path(lib_name);

  // create file
  fs::create_directory(path.parent_path());
  std::ofstream f(path.string().c_str());
  f.close();

  // add path to env 
  std::string cmd = cyclus::Env::ModuleEnvVarName() + '=' + \
                    path.parent_path().string();
  putenv((char*)cmd.c_str());

  // test
  DynamicModule mod = DynamicModule("Facility", name);
  EXPECT_EQ(path.string(), mod.path()); // note path calls (private) SetPath()

  fs::remove_all(path);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(DynamicLoadingTests, LoadLibError) {
  using cyclus::DynamicModule;
  DynamicModule mod = DynamicModule("Facility", "not_a_fac");
  EXPECT_THROW(mod.Initialize(), cyclus::IOError);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(DynamicLoadingTests, ConstructTestFacility) {
  using cyclus::Model;
  cyclus::EventManager em;
  cyclus::Timer ti;
  cyclus::Context ctx(&ti, &em);
  EXPECT_NO_THROW(Model::LoadModule("Facility", "TestFacility");
                  Model* fac = Model::ConstructModel(&ctx, "TestFacility");
                  Model::DeleteModel(fac);
                  Model::UnloadModules(););
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(DynamicLoadingTests, cloneTestFacility) {
  using cyclus::Model;
  using cyclus::Prototype;
  cyclus::EventManager em;
  cyclus::Timer ti;
  cyclus::Context ctx(&ti, &em);
  EXPECT_NO_THROW(Model::LoadModule("Facility", "TestFacility");
                  Model* fac = Model::ConstructModel(&ctx, "TestFacility");
                  Prototype* clone = dynamic_cast<Prototype*>(fac)->clone();
                  Model::DeleteModel(dynamic_cast<Model*>(clone));
                  Model::DeleteModel(fac);
                  Model::UnloadModules(););
}
