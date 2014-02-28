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

TEST(DynamicLoadingTests, ConstructTestFacility) {
  cyclus::Recorder rec;
  cyclus::Timer ti;
  cyclus::Context* ctx = new cyclus::Context(&ti, &rec);
  EXPECT_NO_THROW(DynamicModule::Make(ctx, "TestFacility"));
  EXPECT_NO_THROW(DynamicModule::CloseAll());
}

TEST(DynamicLoadingTests, LoadLibError) {
  cyclus::Recorder rec;
  cyclus::Timer ti;
  cyclus::Context* ctx = new cyclus::Context(&ti, &rec);
  EXPECT_THROW(DynamicModule::Make(ctx, "not_a_fac"), cyclus::IOError);
}

TEST(DynamicLoadingTests, CloneTestFacility) {
  cyclus::Recorder rec;
  cyclus::Timer ti;
  cyclus::Context* ctx = new cyclus::Context(&ti, &rec);
  EXPECT_NO_THROW(
                  Model* fac = DynamicModule::Make(ctx, "TestFacility");
                  Model* clone = fac->Clone();
                  );
  DynamicModule::CloseAll();
}

