
#include <gtest/gtest.h>

#include "toolkit/matl_sell_policy.h"

#include "composition.h"
#include "material.h"
#include "bid.h"
#include "request.h"
#include "error.h"
#include "pyne.h"
#include "package.h"

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
  ResBuf<Material> buff;
  Composition::Ptr comp, comp1;
  Material::Ptr mat, mat1;
  
  virtual void SetUp() {
    fac1 = new TestFacility(tc.get());
    cap = 5;
    qty = 3;
    buff.capacity(cap);
    CompMap v;
    v[pyne::nucname::id("H1")] = 1;
    comp = Composition::CreateFromAtom(v);
    mat = Material::CreateUntracked(qty, comp);
    buff.Push(mat);
    comp1 = Composition::CreateFromAtom(v);
    mat1 = Material::CreateUntracked(qty, comp1);
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

  // limit & excl
  p.Init(fac1, &buff, "", 1.5, false, 1);
  ASSERT_TRUE(p.Excl());
  ASSERT_FLOAT_EQ(p.Limit(), 1.5);

  // throughput = 2.5, quantize = 2, limit = 2
  p.Init(fac1, &buff, "", qty - 0.5, false, qty - 1);
  ASSERT_FLOAT_EQ(p.Limit(), qty - 1);
}

TEST_F(MatlSellPolicyTests, StartStop) {
  MatlSellPolicy p;
  p.Init(NULL, &buff, "");
  ASSERT_THROW(p.Start(), ValueError);
  ASSERT_THROW(p.Stop(), ValueError);
}

TEST_F(MatlSellPolicyTests, Bids) {
  MatlSellPolicy p;
  std::string commod("commod");  
  CommodMap<Material>::type reqs;
  reqs[commod] = std::vector<Request<Material>*>();
  Request<Material>* req = Request<Material>::Create(mat1, fac1, commod);
  reqs[commod].push_back(req);
  std::set<BidPortfolio<Material>::Ptr> obs;

  // basic
  p.Init(NULL, &buff, "").Set(commod);
  obs = p.GetMatlBids(reqs);
  ASSERT_EQ(obs.size(), 1);
  ASSERT_EQ((*obs.begin())->bids().size(), 1);
  ASSERT_FLOAT_EQ((*(*obs.begin())->bids().begin())->offer()->quantity(),
                  mat->quantity());
  ASSERT_EQ((*(*obs.begin())->bids().begin())->offer()->comp(), comp);

  // excl and ignore_comp
    // Qty = 3, Throughput = 3, Quanta = 1.5 -> 2 transactions of 1 quanta
  p.Init(NULL, &buff, "", qty, true, qty / 2).Set(commod);
  obs = p.GetMatlBids(reqs);
  ASSERT_EQ(obs.size(), 1);
  ASSERT_EQ((*obs.begin())->bids().size(), 2);
  ASSERT_FLOAT_EQ((*(*obs.begin())->bids().begin())->offer()->quantity(),
                  qty / 2);
  ASSERT_EQ((*(*obs.begin())->bids().begin())->offer()->comp(), comp1);

  // If available quantity is bigger than the quanta only an integer number of quanta is offered
  // Qty = 3, Throughput = 3, Quanta = 2 -> only 1 transactions of 1 quanta
  p.Init(NULL, &buff, "", qty, true, 2).Set(commod);
  obs = p.GetMatlBids(reqs);
  ASSERT_EQ(obs.size(), 1);
  ASSERT_EQ((*obs.begin())->bids().size(), 1);
  ASSERT_FLOAT_EQ((*(*obs.begin())->bids().begin())->offer()->quantity(),
                  2);
  ASSERT_EQ((*(*obs.begin())->bids().begin())->offer()->comp(), comp1);
  

  // quantize bigger than the quantity in storage
   // Qty = 3, Throughput = 3, Quanta = 6 -> No transaction
  p.Init(NULL, &buff, "", qty, true, qty * 2).Set(commod);
  obs = p.GetMatlBids(reqs);
  ASSERT_EQ(obs.size(), 0);
  delete req;
}

TEST_F(MatlSellPolicyTests, Trades) {
  MatlSellPolicy p;
  std::string commod("commod");  
  std::vector<Trade<Material> > trades;
  std::vector<std::pair<Trade<Material>, Material::Ptr> > obs;
  
  Request<Material>* req = Request<Material>::Create(mat1, fac1, commod);
  Bid<Material>* bid = Bid<Material>::Create(req, mat, fac1);
  Trade<Material> trade(req, bid, 1);
  trades.push_back(trade);
  
  // basic
  p.Init(NULL, &buff, "").Set(commod);
  p.GetMatlTrades(trades, obs);
  ASSERT_EQ(obs.size(), 1);
  ASSERT_EQ(obs.begin()->second->comp(), comp);
  
  // ignore comp
  obs.clear();
  p.Init(NULL, &buff, "", qty, true).Set(commod);
  p.GetMatlTrades(trades, obs);
  ASSERT_EQ(obs.size(), 1);
  ASSERT_EQ(obs.begin()->second->comp(), comp1);

  delete bid;
  delete req;
}

TEST_F(MatlSellPolicyTests, Package) {
  using cyclus::QueryResult;

  int dur = 5;
  double throughput = 1;

  cyclus::MockSim sim(dur);
  cyclus::Agent* a = new TestFacility(sim.context());
  
  sim.context()->AddPrototype(a->prototype(), a);
  sim.agent = sim.context()->CreateAgent<cyclus::Agent>(a->prototype());
  sim.AddSink("commod").Finalize();
  TestFacility* fac = dynamic_cast<TestFacility*>(sim.agent);

  cyclus::toolkit::ResBuf<cyclus::Material> buf;

  double qty = 5;
  CompMap cm;
  cm[922350000] = 0.05;
  cm[922380000] = 0.95;
  Composition::Ptr comp = Composition::CreateFromMass(cm);
  mat = Material::Create(a, qty, comp, Package::unpackaged_name());

  buf.Push(mat);

  sim.context()->AddPackage("foo", 1, 2, "first");
  Package::Ptr p = sim.context()->GetPackage("foo");

  cyclus::toolkit::MatlSellPolicy sellpol;
  sellpol.Init(fac, &buf, "buf", 4, false, 0, p->name())
          .Set("commod").Start();

  EXPECT_NO_THROW(sim.Run());

  QueryResult qr_trans = sim.db().Query("Transactions", NULL);
  QueryResult qr_res = sim.db().Query("Resources", NULL);
  EXPECT_EQ(3, qr_trans.rows.size());

  EXPECT_EQ(0, qr_trans.GetVal<int>("Time", 0));
  EXPECT_EQ(0, qr_trans.GetVal<int>("Time", 1));
  EXPECT_EQ(1, qr_trans.GetVal<int>("Time", 2));

  // Resource 0 is the material of 5 that we first created. Resource 1/2 is 
  // the first resource split into 3 and 2. Resource 3/4 is split into 1 and 2
  EXPECT_NEAR(2, qr_res.GetVal<double>("Quantity", 2), 0.00001);
  EXPECT_NEAR(2, qr_res.GetVal<double>("Quantity", 4), 0.00001);

  // All material should have been transacted, including the resource of size 1
  EXPECT_EQ(0, buf.quantity());
}
}
}