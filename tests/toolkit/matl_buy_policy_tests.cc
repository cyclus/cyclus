
#include <gtest/gtest.h>

#include "toolkit/matl_buy_policy.h"

#include "composition.h"
#include "material.h"
#include "request.h"
#include "error.h"
#include "toolkit/resource_buff.h"

#include "test_context.h"
#include "test_agents/test_facility.h"
#include "cyclus.h"

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
  cyclus::Timer ti;
  cyclus::Recorder rec;
  cyclus::Context* ctx;

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

// Tests that matlbuypolicy sends out a request properly
TEST_F(MatlBuyPolicyTests, OneReq) {
  double cap = 5;
  ResBuf<Material> buff;
  buff.capacity(cap);
  cyclus::Composition::Ptr c1 = cyclus::Composition::Ptr(new TestComp()); 
  MatlBuyPolicy p;

  p.Init(fac1, &buff, "").Set("commod1", c1);
  std::set<RequestPortfolio<Material>::Ptr> obs = p.GetMatlRequests();
  ASSERT_EQ(obs.size(), 1);
  ASSERT_EQ((*obs.begin())->requests().size(), 1);
  Request<Material>* req = (*obs.begin())->requests().at(0);
  ASSERT_EQ(req->commodity(), "commod1");
  ASSERT_FLOAT_EQ(req->target()->quantity(), cap);
}

// Tests that matlbuypolicy can send out requests for multiple commodities
TEST_F(MatlBuyPolicyTests, MultipleReqs) {
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
}

// Test single quantized request
TEST_F(MatlBuyPolicyTests, Quantize) {
  double cap = 5;
  ResBuf<Material> buff;
  buff.capacity(cap);
  double p2 = 2.5;
  cyclus::Composition::Ptr c1 = cyclus::Composition::Ptr(new TestComp()); 
  MatlBuyPolicy p;

  double quantize = 2.5;
  p.Init(fac1, &buff, "", std::numeric_limits<double>::max(), 1, 1, quantize).Set("commod1", c1);
  std::set<RequestPortfolio<Material>::Ptr> obs = p.GetMatlRequests();
  ASSERT_EQ(obs.size(), 2);
  ASSERT_EQ((*obs.begin())->requests().size(), 1);
  Request<Material>* req = (*obs.begin())->requests().at(0);
  ASSERT_TRUE(req->exclusive());
  ASSERT_FLOAT_EQ(req->target()->quantity(), quantize);
}

// Test two quantized requests
TEST_F(MatlBuyPolicyTests, MultiReqQuantize) {
    double cap = 5;
  ResBuf<Material> buff;
  buff.capacity(cap);
  cyclus::Composition::Ptr c1 = cyclus::Composition::Ptr(new TestComp()); 
  cyclus::Composition::Ptr c2 = cyclus::Composition::Ptr(new TestComp()); 
  MatlBuyPolicy p;
  
  // two portfolios with quantize
  double quantize = 2.5;
  p.Init(fac1, &buff, "", std::numeric_limits<double>::max(), 1, 1, quantize).Set("commod1", c1).Set("commod2", c2);
  std::set<RequestPortfolio<Material>::Ptr> obs = p.GetMatlRequests();
  ASSERT_EQ(obs.size(), 2);
  ASSERT_EQ((*obs.begin())->requests().size(), 2);
  ASSERT_EQ((*(obs.begin()++))->requests().size(), 2);
  Request<Material>* req = (*obs.begin())->requests().at(0);
  ASSERT_TRUE(req->exclusive());
  ASSERT_FLOAT_EQ(req->target()->quantity(), quantize);

  req = (*(obs.begin()++))->requests().at(0);
  ASSERT_TRUE(req->exclusive());
  ASSERT_FLOAT_EQ(req->target()->quantity(), quantize);
}

// Tests that agent cycles between active and dormant cycles during a mock
// sim, only buying when active.
TEST_F(MatlBuyPolicyTests, ActiveDormant) {
  int active = 2;
  int dormant = 3;
  int dur = 4;
  double throughput = 1;

  cyclus::MockSim sim(dur);
  cyclus::Agent* a = new TestFacility(sim.context());
  sim.context()->AddPrototype(a->prototype(), a);
  sim.agent = sim.context()->CreateAgent<cyclus::Agent>(a->prototype());
  sim.AddSource("commod1").Finalize();

  TestFacility* fac = dynamic_cast<TestFacility*>(sim.agent);

  cyclus::toolkit::ResBuf<cyclus::Material> inbuf;
  cyclus::toolkit::MatlBuyPolicy policy;
  policy.Init(fac, &inbuf, "inbuf", throughput, active, dormant)
        .Set("commod1").Start();

  EXPECT_NO_THROW(sim.Run());
  EXPECT_DOUBLE_EQ(2, inbuf.quantity());

  delete a;
}

TEST_F(MatlBuyPolicyTests, ActiveDormantMultipleCycles) {
  using cyclus::QueryResult;

  int active = 2;
  int dormant = 3;
  int dur = 16;
  double throughput = 1;

  cyclus::MockSim sim(dur);
  cyclus::Agent* a = new TestFacility(sim.context());
  sim.context()->AddPrototype(a->prototype(), a);
  sim.agent = sim.context()->CreateAgent<cyclus::Agent>(a->prototype());
  sim.AddSource("commod1").Finalize();

  TestFacility* fac = dynamic_cast<TestFacility*>(sim.agent);

  cyclus::toolkit::ResBuf<cyclus::Material> inbuf;
  cyclus::toolkit::MatlBuyPolicy policy;
  policy.Init(fac, &inbuf, "inbuf", throughput, active, dormant)
        .Set("commod1").Start();

  EXPECT_NO_THROW(sim.Run());

  QueryResult qr = sim.db().Query("Transactions", NULL);
  // confirm that transactions are only occurring during active periods
  int first_cycle = qr.GetVal<int>("Time", 0);
  EXPECT_EQ(0, first_cycle);
  int second_cycle = qr.GetVal<int>("Time", 2);
  EXPECT_EQ(5, second_cycle);
  int third_cycle = qr.GetVal<int>("Time", 5);
  EXPECT_EQ(11, third_cycle);

  // confirm total inbuf recieved material
  EXPECT_DOUBLE_EQ(7, inbuf.quantity());

  delete a;
}

}
}
