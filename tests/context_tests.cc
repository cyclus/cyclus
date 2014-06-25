#include <gtest/gtest.h>

#include "context.h"
#include "recorder.h"
#include "test_agents/test_facility.h"
#include "timer.h"

using cyclus::Context;
using cyclus::Recorder;
using cyclus::Agent;
using cyclus::Timer;
using cyclus::Trader;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class ContextTests: public ::testing::Test {
 protected:
  Timer ti;
  Recorder rec;
  Context* ctx;
  TestFacility* fac;

  virtual void SetUp() {
    ctx = new Context(&ti, &rec);
    fac = new TestFacility(ctx);
  }

  virtual void TearDown() {
    delete fac;
    delete ctx;
  }
};

class DonutShop : public Agent {
 public:
  DonutShop(Context* ctx, std::string dotd)
      : Agent(ctx),
        donut_of_the_day(dotd) {}

  virtual ~DonutShop() {
    destruct_count++;
  }

  virtual Agent* Clone() {
    DonutShop* m = new DonutShop(context(), donut_of_the_day);
    m->InitFrom(this);
    return m;
  }

  void InitFrom(DonutShop* m) {
    Agent::InitFrom(m);
  }

  virtual void Snapshot(cyclus::DbInit di) {}
  virtual void InitInv(cyclus::Inventories& inv) {}
  virtual cyclus::Inventories SnapshotInv() {return cyclus::Inventories();}

  static int destruct_count;
  std::string donut_of_the_day;
};

int DonutShop::destruct_count = 0;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ContextTests, EnterLeave) {
  EXPECT_TRUE(ctx->traders().empty());
  Agent* clone = fac->Clone();
  EXPECT_EQ(ctx->n_prototypes(TestFacility::proto_name()), 0);
  EXPECT_EQ(ctx->n_specs(TestFacility::spec()), 0);
  clone->Build(NULL);
  Trader* exr = dynamic_cast<Trader*>(clone);
  EXPECT_EQ(ctx->traders().size(), 1);
  EXPECT_EQ(*ctx->traders().begin(), exr);
  EXPECT_EQ(ctx->n_prototypes(TestFacility::proto_name()), 1);
  EXPECT_EQ(ctx->n_specs(TestFacility::spec()), 1);
  EXPECT_EQ(*ctx->traders().begin(), exr);
  clone->Decommission();
  EXPECT_EQ(ctx->n_prototypes(TestFacility::proto_name()), 0);
  EXPECT_EQ(ctx->n_specs(TestFacility::spec()), 0);
  EXPECT_TRUE(ctx->traders().empty());
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ContextTests, CreateAgent) {
  Timer ti;
  Recorder rec;
  Context* ctx = new Context(&ti, &rec);

  Agent* m1 = new DonutShop(ctx, "old fashion");
  ctx->AddPrototype("dunkin donuts", m1);
  Agent* m2 = new DonutShop(ctx, "apple fritter");
  ctx->AddPrototype("krispy kreme", m2);
  Agent* m3 = new DonutShop(ctx, "raspberry filled");
  ctx->AddPrototype("greenbush bakery", m3);

  DonutShop* d;
  ASSERT_NO_THROW(d = ctx->CreateAgent<DonutShop>("dunkin donuts"));
  EXPECT_EQ("old fashion", d->donut_of_the_day);
  EXPECT_NE(d, m1);

  ASSERT_NO_THROW(d = ctx->CreateAgent<DonutShop>("krispy kreme"));
  EXPECT_EQ("apple fritter", d->donut_of_the_day);
  EXPECT_NE(d, m2);

  ASSERT_THROW(ctx->CreateAgent<cyclus::Context>("krispy kreme"),
               cyclus::CastError);

  delete ctx;

  EXPECT_EQ(6, DonutShop::destruct_count);
}
