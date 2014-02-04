#include <gtest/gtest.h>

#include <stdlib.h>
#include <fstream>
#include <iostream>
#include "boost/filesystem.hpp"

#include "context.h"
#include "env.h"
#include "error.h"
#include "recorder.h"
#include "model.h"
#include "timer.h"
#include "dynamic_module.h"

namespace fs = boost::filesystem;
using cyclus::DynamicModule;
using cyclus::Model;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(DynamicLoadingTests, LoadTestFacility) {
  cyclus::DynamicModule* m;
  EXPECT_NO_THROW(m = new DynamicModule("TestFacility"));
  EXPECT_NO_THROW(m->CloseLibrary());
  EXPECT_NO_THROW(delete m);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(DynamicLoadingTests, LoadLibError) {
  EXPECT_THROW(new DynamicModule("not_a_fac"), cyclus::IOError);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(DynamicLoadingTests, ConstructTestFacility) {
  cyclus::Recorder rec;
  cyclus::Timer ti;
  cyclus::DynamicModule module("TestFacility");
  cyclus::Context* ctx = new cyclus::Context(&ti, &rec);
  EXPECT_NO_THROW(
                  Model* fac = module.ConstructInstance(ctx);
                  );
  delete ctx;
  module.CloseLibrary();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST(DynamicLoadingTests, cloneTestFacility) {
  cyclus::Recorder rec;
  cyclus::Timer ti;
  cyclus::DynamicModule module("TestFacility");
  cyclus::Context* ctx = new cyclus::Context(&ti, &rec);
  EXPECT_NO_THROW(
                  Model* fac = module.ConstructInstance(ctx);
                  Model* clone = fac->Clone();
                  );
  delete ctx;
  module.CloseLibrary();
}
