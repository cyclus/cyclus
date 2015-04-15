
#include <gtest/gtest.h>

#include "toolkit/matl_sell_policy.h"

#include "composition.h"
#include "material.h"
#include "request.h"
#include "error.h"
#include "pyne.h"
#include "toolkit/resource_buff.h"

#include "test_context.h"
#include "test_agents/test_facility.h"

namespace cyclus {
namespace toolkit {

class TestComp : public Composition {
 public:
  TestComp() {}
};

class MatlSellPolicyTests: public ::testing::Test {
 protected:
  TestContext tc;
  TestFacility* fac1;
  double cap, qty;
  ResourceBuff buff;

  virtual void SetUp() {
    fac1 = new TestFacility(tc.get());
    cap = 5;
    qty = 3;
    buff.set_capacity(cap);
    CompMap v;
    v[pyne::nucname::id("H1")] = 1;
    buff.Push(Material::CreateUntracked(qty, Composition::CreateFromAtom(v)));
  }

  virtual void TearDown() {
    delete fac1;
  }
};


TEST_F(MatlSellPolicyTests, Init) {
  MatlSellPolicy p;

  // defaults
  p.Init(fac1, &buff, "");
  ASSERT_FALSE(p.Excl());
  ASSERT_FLOAT_EQ(p.Limit(), qty);

  // limit
  p.Init(fac1, &buff, "", 1.5, false, 1);
  ASSERT_TRUE(p.Excl());
  ASSERT_FLOAT_EQ(p.Limit(), 1.5);
  
  // test bad state
  ASSERT_DEATH(p.Init(fac1, &buff, "", 0, false),
               "Assertion `x > 0' failed");
  ASSERT_DEATH(p.Init(fac1, &buff, "", 1, false, 0),
               "Assertion `x != 0' failed");
}


}
}
