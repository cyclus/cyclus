
#include <gtest/gtest.h>

#include "context.h"
#include "event_manager.h"
#include "mock_facility.h"
#include "timer.h"

using cyclus::Timer;
using cyclus::Model;
using cyclus::Exchanger;
using cyclus::EventManager;
using cyclus::Context;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class ContextTests: public ::testing::Test {
 protected:
  Timer ti;
  EventManager em;
  Context* ctx;
  MockFacility* fac;
  
  virtual void SetUp() {
    ctx = new Context(&ti, &em);
    fac = new MockFacility(ctx);
  };
  
  virtual void TearDown() {
    delete fac;
    delete ctx;
  };
  
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ContextTests, exchangers) {
  EXPECT_TRUE(ctx->exchangers().empty());
  Model* clone = fac->Clone();
  clone->Deploy(clone);
  Exchanger* exr = dynamic_cast<Exchanger*>(clone);
  EXPECT_EQ(ctx->exchangers().size(), 1);
  EXPECT_EQ(*ctx->exchangers().begin(), exr);
  clone->Decommission();
  EXPECT_TRUE(ctx->exchangers().empty());
}  
