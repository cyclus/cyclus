#include <gtest/gtest.h>

#include <stdlib.h>
#include <fstream>
#include <iostream>
#include "boost/filesystem.hpp"

#include "context.h"
#include "env.h"
#include "error.h"
#include "recorder.h"
#include "agent.h"
#include "timer.h"
#include "dynamic_module.h"

namespace fs = boost::filesystem;
using cyclus::DynamicModule;
using cyclus::AgentSpec;
using cyclus::Agent;

TEST(DynamicLoadingTests, ConstructTestFacility) {
  cyclus::Recorder rec;
  cyclus::Timer ti;
  cyclus::Context* ctx = new cyclus::Context(&ti, &rec);
  EXPECT_NO_THROW(DynamicModule::Make(ctx, AgentSpec("tests:TestFacility:TestFacility")));
  EXPECT_NO_THROW(DynamicModule::CloseAll());
}

TEST(DynamicLoadingTests, LoadLibError) {
  cyclus::Recorder rec;
  cyclus::Timer ti;
  cyclus::Context* ctx = new cyclus::Context(&ti, &rec);
  EXPECT_THROW(DynamicModule::Make(ctx, AgentSpec("foo:foo:not_a_fac")), cyclus::IOError);
}

TEST(DynamicLoadingTests, CloneTestFacility) {
  cyclus::Recorder rec;
  cyclus::Timer ti;
  cyclus::Context* ctx = new cyclus::Context(&ti, &rec);
  EXPECT_NO_THROW(Agent* fac = DynamicModule::Make(ctx, AgentSpec("tests:TestFacility:TestFacility"));
                  Agent* clone = fac->Clone(););
  DynamicModule::CloseAll();
}
