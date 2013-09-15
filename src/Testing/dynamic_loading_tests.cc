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
#include "timer.h"
#include "dynamic_module.h"

namespace fs = boost::filesystem;
using cyclus::DynamicModule;
using cyclus::Model;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(DynamicLoadingTests, LoadTestFacility) {
  cyclus::DynamicModule* m;
  EXPECT_NO_THROW(m = new DynamicModule("Facility", "TestFacility"));
  EXPECT_NO_THROW(m->CloseLibrary());
  EXPECT_NO_THROW(delete m);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(DynamicLoadingTests, FindNonStandardPath) {
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
  DynamicModule mod("Facility", name);
  EXPECT_EQ(path.string(), mod.path()); // note path calls (private) SetPath()

  fs::remove_all(path);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(DynamicLoadingTests, LoadLibError) {
  EXPECT_THROW(new DynamicModule("Facility", "not_a_fac"), cyclus::IOError);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(DynamicLoadingTests, ConstructTestFacility) {
  cyclus::EventManager em;
  cyclus::Timer ti;
  cyclus::Context ctx(&ti, &em);
  cyclus::DynamicModule* module = new DynamicModule("Facility", "TestFacility");
  EXPECT_NO_THROW(
                  Model* fac = module->ConstructInstance(&ctx);
                  delete fac;
                  );
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(DynamicLoadingTests, cloneTestFacility) {
  cyclus::EventManager em;
  cyclus::Timer ti;
  cyclus::Context ctx(&ti, &em);
  cyclus::DynamicModule* module = new DynamicModule("Facility", "TestFacility");
  EXPECT_NO_THROW(
                  Model* fac = module->ConstructInstance(&ctx);
                  Model* clone = fac->clone();
                  delete clone;
                  delete fac;
                  );
}
