
#include <gtest/gtest.h>

#include "context.h"
#include "recorder.h"
#include "mock_facility.h"
#include "timer.h"

using cyclus::Context;
using cyclus::Recorder;
using cyclus::Model;
using cyclus::Timer;
using cyclus::Trader;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class ContextTests: public ::testing::Test {
 protected:
  Timer ti;
  Recorder rec;
  Context* ctx;
  MockFacility* fac;
  
  virtual void SetUp() {
    ctx = new Context(&ti, &rec);
    fac = new MockFacility(ctx);
  };
  
  virtual void TearDown() {
    delete ctx;
  };
  
};

class DonutShop : public Model {
 public:
  DonutShop(Context* ctx, std::string dotd)
      : Model(ctx), donut_of_the_day(dotd) { };

  virtual ~DonutShop() {
    destruct_count++;
  }

  virtual Model* Clone() {
    DonutShop* m = new DonutShop(*this);
    m->InitFrom(this);
    return m;
  };

  static int destruct_count;
  std::string donut_of_the_day;
};

int DonutShop::destruct_count = 0;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ContextTests, traders) {
  EXPECT_TRUE(ctx->traders().empty());
  Model* clone = fac->Clone();
  clone->Deploy();
  Trader* exr = dynamic_cast<Trader*>(clone);
  EXPECT_EQ(ctx->traders().size(), 1);
  EXPECT_EQ(*ctx->traders().begin(), exr);
  clone->Decommission();
  EXPECT_TRUE(ctx->traders().empty());
}  

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
TEST_F(ContextTests, CreateModel) {
  Timer ti;
  Recorder rec;
  Context ctx(&ti, &rec);

  Model* m1 = new DonutShop(&ctx, "old fashion");
  ctx.AddPrototype("dunkin donuts", m1);
  Model* m2 = new DonutShop(&ctx, "apple fritter");
  ctx.AddPrototype("krispy kreme", m2);
  Model* m3 = new DonutShop(&ctx, "raspberry filled");
  ctx.AddPrototype("greenbush bakery", m3);

  DonutShop* d;
  ASSERT_NO_THROW(d = ctx.CreateModel<DonutShop>("dunkin donuts"));
  EXPECT_EQ("old fashion", d->donut_of_the_day);
  EXPECT_NE(d, m1);

  ASSERT_NO_THROW(d = ctx.CreateModel<DonutShop>("krispy kreme"));
  EXPECT_EQ("apple fritter", d->donut_of_the_day);
  EXPECT_NE(d, m2);

  ASSERT_THROW(ctx.CreateModel<cyclus::Context>("krispy kreme"), cyclus::CastError);

  EXPECT_EQ(5, DonutShop::destruct_count);
}  

