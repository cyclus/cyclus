
#include <gtest/gtest.h>

#include "toolkit/matl_buy_policy.h"

#include "composition.h"
#include "material.h"
#include "request.h"
#include "error.h"
#include "toolkit/resource_buff.h"

#include "test_context.h"
#include "test_agents/test_facility.h"

namespace cyclus {
namespace toolkit {

class TestComp : public Composition {
 public:
  TestComp() {}
};

class MatlBuyPolicyTests: public ::testing::Test {
 protected:
  TestContext tc;
  TestFacility* fac1;

  virtual void SetUp() {
    fac1 = new TestFacility(tc.get());
  }

  virtual void TearDown() {
    delete fac1;
  }
};

TEST_F(MatlBuyPolicyTests, Init) {
  double cap = 5;
  ResBuf<Material> buff;
  buff.capacity(cap);
  MatlBuyPolicy p;

  // defaults
  p.Init(fac1, &buff, "");
  ASSERT_FLOAT_EQ(p.TotalQty(), cap);
  ASSERT_FLOAT_EQ(p.ReqQty(), cap);
  ASSERT_EQ(p.NReq(), 1);

  // throughput
  double throughput = cap - 1;
  p.Init(fac1, &buff, "", throughput, 1, 1, -1);
  ASSERT_FLOAT_EQ(p.TotalQty(), throughput);
  ASSERT_FLOAT_EQ(p.ReqQty(), throughput);
  ASSERT_EQ(p.NReq(), 1);

  // exclusive orders
  double quantize = 2.5;
  p.Init(fac1, &buff, "", std::numeric_limits<double>::max(), 1, 1, quantize);
  ASSERT_FLOAT_EQ(p.TotalQty(), cap);
  ASSERT_FLOAT_EQ(p.ReqQty(), quantize);
  ASSERT_EQ(p.NReq(), static_cast<int>(cap / quantize));

  // S,s with nothing in buffer 
  double S = 4, s = 2;
  // reset
  p.Init(fac1, &buff, "", std::numeric_limits<double>::max(), 1, 1, -1); 
  // use Ss constructor
  p.Init(fac1, &buff, "", S, s);
  ASSERT_FLOAT_EQ(p.TotalQty(), S);
  ASSERT_FLOAT_EQ(p.ReqQty(), S);
  ASSERT_EQ(p.NReq(), 1);

  // S,s with something in buffer
  Composition::Ptr c;
  buff.Push(Material::CreateUntracked(s, c));
  ASSERT_FLOAT_EQ(p.TotalQty(), S - s);
  ASSERT_FLOAT_EQ(p.ReqQty(), S - s);
  ASSERT_EQ(p.NReq(), 1);
}

TEST_F(MatlBuyPolicyTests, StartStop) {
  double cap = 5;
  ResBuf<Material> buff;
  buff.capacity(cap);
  MatlBuyPolicy p;
  p.Init(NULL, &buff, "");
  ASSERT_THROW(p.Start(), ValueError);
  ASSERT_THROW(p.Stop(), ValueError);
}

TEST_F(MatlBuyPolicyTests, Reqs) {
  double cap = 5;
  ResBuf<Material> buff;
  buff.capacity(cap);
  std::string commod1("foo"), commod2("bar");
  double p2 = 2.5;
  cyclus::Composition::Ptr c1 = cyclus::Composition::Ptr(new TestComp()); 
  cyclus::Composition::Ptr c2 = cyclus::Composition::Ptr(new TestComp()); 
  MatlBuyPolicy p;
  
  // two requests
  p.Init(fac1, &buff, "").Set(commod1, c1).Set(commod2, c2, p2);
  std::set<RequestPortfolio<Material>::Ptr> obs = p.GetMatlRequests();
  ASSERT_EQ(obs.size(), 1);
  ASSERT_EQ((*obs.begin())->requests().size(), 2);
  Request<Material>* req = (*obs.begin())->requests().at(0);
  if (req->commodity() == commod1) {
    ASSERT_FLOAT_EQ(req->preference(), 1.);
    ASSERT_FLOAT_EQ((*obs.begin())->requests().at(1)->preference(), p2);
  } else {
    ASSERT_FLOAT_EQ(req->preference(), p2);
    ASSERT_FLOAT_EQ((*obs.begin())->requests().at(1)->preference(), 1.);
  }
  ASSERT_FALSE(req->exclusive());
  ASSERT_FLOAT_EQ(req->target()->quantity(), cap);
  
  // two portfolios with quantize
  double quantize = 2.5;
  p.Init(fac1, &buff, "", std::numeric_limits<double>::max(), 1, 1, quantize);
  obs = p.GetMatlRequests();
  ASSERT_EQ(obs.size(), 2);
  ASSERT_EQ((*obs.begin())->requests().size(), 2);
  ASSERT_EQ((*(obs.begin()++))->requests().size(), 2);
  req = (*obs.begin())->requests().at(0);
  ASSERT_TRUE(req->exclusive());
  ASSERT_FLOAT_EQ(req->target()->quantity(), quantize);
}

}
}
