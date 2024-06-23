
#include <gtest/gtest.h>

#include "toolkit/matl_buy_policy.h"

#include "composition.h"
#include "material.h"
#include "request.h"
#include "error.h"
#include "random_number_generator.h"

#include "test_context.h"
#include "test_agents/test_facility.h"
#include "cyclus.h"

#include "boost/shared_ptr.hpp"

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
  TotalInvTracker buff_tracker({&buff});
  MatlBuyPolicy p;

  // defaults
  p.Init(fac1, &buff, "", &buff_tracker);
  double amt = p.TotalAvailable();
  ASSERT_FLOAT_EQ(amt, cap);
  ASSERT_FLOAT_EQ(p.ReqQty(amt), cap);
  ASSERT_EQ(p.NReq(amt), 1);

  // throughput
  double throughput = cap - 1;
  p.Init(fac1, &buff, "", &buff_tracker, throughput);
  amt = p.TotalAvailable();
  ASSERT_FLOAT_EQ(amt, throughput);
  ASSERT_FLOAT_EQ(p.ReqQty(amt), throughput);
  ASSERT_EQ(p.NReq(amt), 1);

  // exclusive orders
  double quantize = 2.5;
  p.Init(fac1, &buff, "", &buff_tracker, std::numeric_limits<double>::max(), 
         quantize);
  amt = p.TotalAvailable();
  ASSERT_FLOAT_EQ(amt, cap);
  ASSERT_FLOAT_EQ(p.ReqQty(amt), quantize);
  ASSERT_EQ(p.NReq(amt), static_cast<int>(cap / quantize));
}

TEST_F(MatlBuyPolicyTests, Init_sS) {
  double cap = 10;
  ResBuf<Material> buff;
  buff.capacity(cap);
  TotalInvTracker buff_tracker({&buff});
  MatlBuyPolicy p;

// S,s with nothing in buffer 
  double S = 4, s = 2;
  p.Init(fac1, &buff, "", &buff_tracker, std::numeric_limits<double>::max()); 
  // use Ss constructor
  p.Init(fac1, &buff, "", &buff_tracker, "sS", S, s);
  ASSERT_TRUE(p.MakeReq());
  double amt = p.TotalAvailable();
  ASSERT_FLOAT_EQ(amt, S);
  ASSERT_FLOAT_EQ(p.ReqQty(amt), S);
  ASSERT_EQ(p.NReq(amt), 1);

  // S,s with something in buffer
  Composition::Ptr c;
  buff.Push(Material::CreateUntracked(s, c));
  p.Init(fac1, &buff, "", &buff_tracker, "sS", S, s);
  ASSERT_TRUE(p.MakeReq());
  amt = p.TotalAvailable();
  ASSERT_FLOAT_EQ(amt, S - s);
  ASSERT_FLOAT_EQ(p.ReqQty(amt), S - s);
  ASSERT_EQ(p.NReq(amt), 1);

  // S,s with too much in the buffer
  buff.Push(Material::CreateUntracked(s, c));
  p.Init(fac1, &buff, "", &buff_tracker, "sS", S, s);
  ASSERT_FALSE(p.MakeReq());
}

TEST_F(MatlBuyPolicyTests, Init_RQ) {
  double cap = 10;
  ResBuf<Material> buff;
  buff.capacity(cap);
  TotalInvTracker buff_tracker({&buff});
  MatlBuyPolicy p;

// R,Q with nothing in buffer
  double R = 2, Q = 4;
  // reset
  p.Init(fac1, &buff, "", &buff_tracker, std::numeric_limits<double>::max()); 
  // use RQ constructor
  p.Init(fac1, &buff, "", &buff_tracker, "RQ", Q, R);
  ASSERT_TRUE(p.MakeReq());
  double amt = p.TotalAvailable();
  ASSERT_FLOAT_EQ(amt, 6);
  ASSERT_FLOAT_EQ(p.ReqQty(amt), Q);
  ASSERT_EQ(p.NReq(amt), 1);

  // R,Q with something in the buffer
  Composition::Ptr c;
  buff.Push(Material::CreateUntracked(R, c));
  p.Init(fac1, &buff, "", &buff_tracker, "RQ", Q, R);
  ASSERT_TRUE(p.MakeReq());
  amt = p.TotalAvailable();
  ASSERT_FLOAT_EQ(amt, 4);
  ASSERT_FLOAT_EQ(p.ReqQty(amt), Q);
  ASSERT_EQ(p.NReq(amt), 1);

  // R,Q with too much in the buffer
  buff.Push(Material::CreateUntracked(R, c));
  p.Init(fac1, &buff, "", &buff_tracker, "RQ", Q, R);  
  ASSERT_FALSE(p.MakeReq());
}

TEST_F(MatlBuyPolicyTests, StartStop) {
  double cap = 5;
  ResBuf<Material> buff;
  buff.capacity(cap);
  TotalInvTracker buff_tracker({&buff});
  MatlBuyPolicy p;
  ASSERT_THROW(p.Init(NULL, &buff, "", &buff_tracker), ValueError);
}

// Tests that matlbuypolicy sends out a request properly
TEST_F(MatlBuyPolicyTests, OneReq) {
  double cap = 5;
  ResBuf<Material> buff;
  buff.capacity(cap);
  TotalInvTracker buff_tracker({&buff});
  cyclus::Composition::Ptr c1 = cyclus::Composition::Ptr(new TestComp()); 
  MatlBuyPolicy p;

  p.Init(fac1, &buff, "", &buff_tracker).Set("commod1", c1);
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
  TotalInvTracker buff_tracker({&buff});
  std::string commod1("foo"), commod2("bar");
  double p2 = 2.5;
  cyclus::Composition::Ptr c1 = cyclus::Composition::Ptr(new TestComp()); 
  cyclus::Composition::Ptr c2 = cyclus::Composition::Ptr(new TestComp()); 
  MatlBuyPolicy p;
  
  // two requests
  p.Init(fac1, &buff, "", &buff_tracker).Set(commod1, c1).Set(commod2, c2, p2);
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
  TotalInvTracker buff_tracker({&buff});
  double p2 = 2.5;
  cyclus::Composition::Ptr c1 = cyclus::Composition::Ptr(new TestComp()); 
  MatlBuyPolicy p;

  double quantize = 2.5;
  p.Init(fac1, &buff, "", &buff_tracker, std::numeric_limits<double>::max(),
         quantize).Set("commod1", c1);
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
  TotalInvTracker buff_tracker({&buff});
  cyclus::Composition::Ptr c1 = cyclus::Composition::Ptr(new TestComp()); 
  cyclus::Composition::Ptr c2 = cyclus::Composition::Ptr(new TestComp()); 
  MatlBuyPolicy p;
  
  // two portfolios with quantize
  double quantize = 2.5;
  p.Init(fac1, &buff, "", &buff_tracker, std::numeric_limits<double>::max(),
         quantize).Set("commod1", c1).Set("commod2", c2);
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

TEST_F(MatlBuyPolicyTests, TotalInvTracker) {
  using cyclus::QueryResult;

  int dur = 5;
  double throughput = 1;

  cyclus::MockSim sim(dur); 
  cyclus::Agent* a = new TestFacility(sim.context());
  sim.context()->AddPrototype(a->prototype(), a);
  sim.agent = sim.context()->CreateAgent<cyclus::Agent>(a->prototype());
  TestFacility* fac = dynamic_cast<TestFacility*>(sim.agent);
  sim.AddSource("commod1").Finalize();

  cyclus::toolkit::ResBuf<cyclus::Material> inbuf;
  inbuf.capacity(10);
  cyclus::toolkit::ResBuf<cyclus::Material> otherbuf;
  otherbuf.capacity(10);

  cyclus::CompMap cm;
  cm[1001] = 1;
  double mat_size = 2;
  cyclus::Material::Ptr mat = cyclus::Material::Create(fac, mat_size, cyclus::Composition::CreateFromAtom(cm));
  otherbuf.Push(mat);

  double total_capacity = 5;
  TotalInvTracker buf_tracker({&inbuf, &otherbuf}, total_capacity);
  cyclus::toolkit::MatlBuyPolicy policy;
  policy.Init(fac, &inbuf, "inbuf", &buf_tracker, throughput)
        .Set("commod1").Start();

  EXPECT_FALSE(buf_tracker.empty());
  EXPECT_EQ(buf_tracker.quantity(), mat_size);
  EXPECT_EQ(buf_tracker.capacity(), total_capacity);
  EXPECT_EQ(buf_tracker.space(), total_capacity - mat_size);

  EXPECT_NO_THROW(sim.Run());

  QueryResult qr = sim.db().Query("Transactions", NULL);
  int n_trans = qr.rows.size();
  EXPECT_EQ(n_trans, 3);

  EXPECT_EQ(buf_tracker.quantity(), total_capacity);
  EXPECT_EQ(buf_tracker.space(), 0);
  EXPECT_EQ(inbuf.space(), inbuf.capacity() - inbuf.quantity());
  EXPECT_EQ(buf_tracker.constrained_buf_space(&inbuf), 0);
}

TEST_F(MatlBuyPolicyTests, DefaultFixedActiveDormant) {
  using cyclus::QueryResult;
  
  int dur = 2;
  double throughput = 1;

  boost::shared_ptr<FixedIntDist> a_d_dist = NULL;

  cyclus::MockSim sim(dur);
  cyclus::Agent* a = new TestFacility(sim.context());
  sim.context()->AddPrototype(a->prototype(), a);
  sim.agent = sim.context()->CreateAgent<cyclus::Agent>(a->prototype());
  sim.AddSource("commod1").Finalize();

  TestFacility* fac = dynamic_cast<TestFacility*>(sim.agent);

  cyclus::toolkit::ResBuf<cyclus::Material> inbuf;
  TotalInvTracker buf_tracker({&inbuf});
  cyclus::toolkit::MatlBuyPolicy policy;
  policy.Init(fac, &inbuf, "inbuf", &buf_tracker, throughput, a_d_dist, a_d_dist, NULL)
        .Set("commod1").Start();

  EXPECT_NO_THROW(sim.Run());

  QueryResult qr = sim.db().Query("Transactions", NULL);
  // confirm that transactions are only occurring during active periods
  int first_cycle = qr.GetVal<int>("Time", 0);
  EXPECT_EQ(0, first_cycle);
  int second_cycle = qr.GetVal<int>("Time", 1);
  EXPECT_EQ(1, second_cycle);

  delete a;
}

TEST_F(MatlBuyPolicyTests, FixedActiveDormant) {
  using cyclus::QueryResult;
  
  // Note: ONLY use a combined active and dormant distribution if you are using
  // a fixed, i.e. not random distribution.
  boost::shared_ptr<FixedIntDist> a_d_dist = boost::shared_ptr<FixedIntDist>(new FixedIntDist(1));
  boost::shared_ptr<FixedDoubleDist> size_dist = boost::shared_ptr<FixedDoubleDist>(new FixedDoubleDist(1.0));
  
  int dur = 3;
  double throughput = 1;

  cyclus::MockSim sim(dur);
  cyclus::Agent* a = new TestFacility(sim.context());
  sim.context()->AddPrototype(a->prototype(), a);
  sim.agent = sim.context()->CreateAgent<cyclus::Agent>(a->prototype());
  sim.AddSource("commod1").Finalize();

  TestFacility* fac = dynamic_cast<TestFacility*>(sim.agent);

  cyclus::toolkit::ResBuf<cyclus::Material> inbuf;
  TotalInvTracker buf_tracker({&inbuf});
  cyclus::toolkit::MatlBuyPolicy policy;
  policy.Init(fac, &inbuf, "inbuf", &buf_tracker, throughput, a_d_dist, a_d_dist, size_dist)
        .Set("commod1").Start();

  EXPECT_NO_THROW(sim.Run());

  QueryResult qr = sim.db().Query("Transactions", NULL);
  // confirm that transactions are only occurring during active periods
  int first_cycle = qr.GetVal<int>("Time", 0);
  EXPECT_EQ(0, first_cycle);
  int second_cycle = qr.GetVal<int>("Time", 1);
  EXPECT_EQ(2, second_cycle);

  delete a;
}

TEST_F(MatlBuyPolicyTests, FixedActiveDormantMultipleCycles) {
  using cyclus::QueryResult;

  boost::shared_ptr<FixedIntDist> a_dist =boost::shared_ptr<FixedIntDist>(new FixedIntDist(2));
  boost::shared_ptr<FixedIntDist> d_dist =boost::shared_ptr<FixedIntDist>(new FixedIntDist(1));
  
  int dur = 8;
  double throughput = 1;

  cyclus::MockSim sim(dur);
  cyclus::Agent* a = new TestFacility(sim.context());
  sim.context()->AddPrototype(a->prototype(), a);
  sim.agent = sim.context()->CreateAgent<cyclus::Agent>(a->prototype());
  sim.AddSource("commod1").Finalize();

  TestFacility* fac = dynamic_cast<TestFacility*>(sim.agent);

  cyclus::toolkit::ResBuf<cyclus::Material> inbuf;
  TotalInvTracker buf_tracker({&inbuf});
  cyclus::toolkit::MatlBuyPolicy policy;
  policy.Init(fac, &inbuf, "inbuf", &buf_tracker, throughput, a_dist, d_dist, NULL)
        .Set("commod1").Start();

  EXPECT_NO_THROW(sim.Run());

  QueryResult qr = sim.db().Query("Transactions", NULL);
  int n_trans = qr.rows.size();
  EXPECT_EQ(6, n_trans);
  // confirm that transactions are only occurring during active periods
  int first_cycle = qr.GetVal<int>("Time", 0);
  EXPECT_EQ(0, first_cycle);
  // second cycle should occur on times 3-4
  int second_cycle = qr.GetVal<int>("Time", 2);
  EXPECT_EQ(3, second_cycle);
  // third cycle should occur on times 6-7
  int third_cycle = qr.GetVal<int>("Time", 5);
  EXPECT_EQ(7, third_cycle);

  delete a;
}

TEST_F(MatlBuyPolicyTests, UniformActiveDormant) {
  using cyclus::QueryResult;

  boost::shared_ptr<UniformIntDist> a_dist = boost::shared_ptr<UniformIntDist>(new UniformIntDist(2, 4));
  boost::shared_ptr<UniformIntDist> d_dist = boost::shared_ptr<UniformIntDist>(new UniformIntDist(1, 2));
  
  int dur = 10;
  double throughput = 1;

  cyclus::MockSim sim(dur);
  cyclus::Agent* a = new TestFacility(sim.context());
  sim.context()->AddPrototype(a->prototype(), a);
  sim.agent = sim.context()->CreateAgent<cyclus::Agent>(a->prototype());
  sim.AddSource("commod1").Finalize();

  TestFacility* fac = dynamic_cast<TestFacility*>(sim.agent);

  cyclus::toolkit::ResBuf<cyclus::Material> inbuf;
  TotalInvTracker buf_tracker({&inbuf});
  cyclus::toolkit::MatlBuyPolicy policy;
  policy.Init(fac, &inbuf, "inbuf", &buf_tracker, throughput, a_dist, d_dist, NULL)
        .Set("commod1").Start();

  EXPECT_NO_THROW(sim.Run());

  QueryResult qr = sim.db().Query("Transactions", NULL);
  // confirm that transactions are only occurring during active periods
  int first_cycle = qr.GetVal<int>("Time", 0);
  EXPECT_EQ(0, first_cycle);
  // second cycle should include time 3
  int second_cycle = qr.GetVal<int>("Time", 2);
  EXPECT_EQ(3, second_cycle);
  // third cycle should include 8
  int third_cycle = qr.GetVal<int>("Time", 5);
  EXPECT_EQ(8, third_cycle);

  delete a;
}

TEST_F(MatlBuyPolicyTests, NormalActiveDormant) {
  using cyclus::QueryResult;

  boost::shared_ptr<NormalIntDist> a_dist = boost::shared_ptr<NormalIntDist>(new NormalIntDist(5, 1, 2, 10));
  boost::shared_ptr<NormalIntDist> d_dist = boost::shared_ptr<NormalIntDist>(new NormalIntDist(3, 0.5, 1, 5));
  
  int dur = 25;
  double throughput = 1;

  cyclus::MockSim sim(dur);
  cyclus::Agent* a = new TestFacility(sim.context());
  sim.context()->AddPrototype(a->prototype(), a);
  sim.agent = sim.context()->CreateAgent<cyclus::Agent>(a->prototype());
  sim.AddSource("commod1").Finalize();

  TestFacility* fac = dynamic_cast<TestFacility*>(sim.agent);

  cyclus::toolkit::ResBuf<cyclus::Material> inbuf;
  TotalInvTracker buf_tracker({&inbuf});
  cyclus::toolkit::MatlBuyPolicy policy;
  policy.Init(fac, &inbuf, "inbuf", &buf_tracker, throughput, a_dist, d_dist, NULL)
        .Set("commod1").Start();

  EXPECT_NO_THROW(sim.Run());

  QueryResult qr = sim.db().Query("Transactions", NULL);
  // confirm that transactions are only occurring during active periods
  int first_cycle = qr.GetVal<int>("Time", 0);
  EXPECT_EQ(0, first_cycle);
  // second cycle should include on time 9
  int second_cycle = qr.GetVal<int>("Time", 6);
  EXPECT_EQ(9, second_cycle);
  // third cycle should include time 19
  int third_cycle = qr.GetVal<int>("Time", 13);
  EXPECT_EQ(19, third_cycle);

  delete a;
}

TEST_F(MatlBuyPolicyTests, MixedActiveDormant) {
  using cyclus::QueryResult;
  
  boost::shared_ptr<NormalIntDist> a_dist = boost::shared_ptr<NormalIntDist>(new NormalIntDist(5, 1, 0, cyclus::CY_LARGE_INT));
  boost::shared_ptr<UniformIntDist> d_dist = boost::shared_ptr<UniformIntDist>(new UniformIntDist(1, 3));
  
  int dur = 12;
  double throughput = 1;

  cyclus::MockSim sim(dur);
  cyclus::Agent* a = new TestFacility(sim.context());
  sim.context()->AddPrototype(a->prototype(), a);
  sim.agent = sim.context()->CreateAgent<cyclus::Agent>(a->prototype());
  sim.AddSource("commod1").Finalize();

  TestFacility* fac = dynamic_cast<TestFacility*>(sim.agent);

  cyclus::toolkit::ResBuf<cyclus::Material> inbuf;
  TotalInvTracker buf_tracker({&inbuf});
  cyclus::toolkit::MatlBuyPolicy policy;
  policy.Init(fac, &inbuf, "inbuf", &buf_tracker, throughput, a_dist, d_dist, NULL)
        .Set("commod1").Start();

  EXPECT_NO_THROW(sim.Run());

  QueryResult qr = sim.db().Query("Transactions", NULL);
  // confirm that transactions are only occurring during active periods
  int first_cycle = qr.GetVal<int>("Time", 5);
  EXPECT_EQ(5, first_cycle);
  // second cycle should start on time 8
  int second_cycle = qr.GetVal<int>("Time", 6);
  EXPECT_EQ(8, second_cycle);

  delete a;
}

TEST_F(MatlBuyPolicyTests, RandomSizeUniform) {
  using cyclus::QueryResult;
  
  boost::shared_ptr<UniformDoubleDist> size_dist = boost::shared_ptr<UniformDoubleDist>(new UniformDoubleDist(0.5, 1.0));
  
  int dur = 2;
  double throughput = 10;

  cyclus::MockSim sim(dur);
  cyclus::Agent* a = new TestFacility(sim.context());
  sim.context()->AddPrototype(a->prototype(), a);
  sim.agent = sim.context()->CreateAgent<cyclus::Agent>(a->prototype());
  sim.AddSource("commod1").Finalize();

  TestFacility* fac = dynamic_cast<TestFacility*>(sim.agent);

  cyclus::toolkit::ResBuf<cyclus::Material> inbuf;
  TotalInvTracker buf_tracker({&inbuf});
  cyclus::toolkit::MatlBuyPolicy policy;
  policy.Init(fac, &inbuf, "inbuf", &buf_tracker, throughput, NULL, NULL, size_dist)
        .Set("commod1").Start();

  EXPECT_NO_THROW(sim.Run());

  QueryResult qr = sim.db().Query("Resources", NULL);
  EXPECT_NEAR(6.59845, qr.GetVal<double>("Quantity", 0), 0.00001);
  EXPECT_NEAR(9.70636, qr.GetVal<double>("Quantity", 1), 0.00001);

  delete a;
}

TEST_F(MatlBuyPolicyTests, RandomSizeNormal) {
  using cyclus::QueryResult;
  
  boost::shared_ptr<NormalDoubleDist> size_dist = boost::shared_ptr<NormalDoubleDist>(new NormalDoubleDist(0.5, 0.1));
  
  int dur = 2;
  double throughput = 10;

  cyclus::MockSim sim(dur);
  cyclus::Agent* a = new TestFacility(sim.context());
  sim.context()->AddPrototype(a->prototype(), a);
  sim.agent = sim.context()->CreateAgent<cyclus::Agent>(a->prototype());
  sim.AddSource("commod1").Finalize();

  TestFacility* fac = dynamic_cast<TestFacility*>(sim.agent);

  cyclus::toolkit::ResBuf<cyclus::Material> inbuf;
  TotalInvTracker buf_tracker({&inbuf});
  cyclus::toolkit::MatlBuyPolicy policy;
  policy.Init(fac, &inbuf, "inbuf", &buf_tracker, throughput, NULL, NULL, size_dist)
        .Set("commod1").Start();

  EXPECT_NO_THROW(sim.Run());

  QueryResult qr = sim.db().Query("Resources", NULL);
  EXPECT_NEAR(6.40838, qr.GetVal<double>("Quantity", 0), 0.00001);
  EXPECT_NEAR(3.26489, qr.GetVal<double>("Quantity", 1), 0.00001);

  delete a;
}

TEST_F(MatlBuyPolicyTests, RandomSizeAndFrequency) {
  using cyclus::QueryResult;
  
  boost::shared_ptr<UniformIntDist> a_dist = boost::shared_ptr<UniformIntDist>(new UniformIntDist(1, 2));
  boost::shared_ptr<UniformIntDist> d_dist = boost::shared_ptr<UniformIntDist>(new UniformIntDist(1, 2));
  boost::shared_ptr<NormalDoubleDist> size_dist = boost::shared_ptr<NormalDoubleDist>(new NormalDoubleDist(0.5, 0.25));
  
  int dur = 8;
  double throughput = 10;

  cyclus::MockSim sim(dur);
  cyclus::Agent* a = new TestFacility(sim.context());
  sim.context()->AddPrototype(a->prototype(), a);
  sim.agent = sim.context()->CreateAgent<cyclus::Agent>(a->prototype());
  sim.AddSource("commod1").Finalize();

  TestFacility* fac = dynamic_cast<TestFacility*>(sim.agent);

  cyclus::toolkit::ResBuf<cyclus::Material> inbuf;
  TotalInvTracker buf_tracker({&inbuf});
  cyclus::toolkit::MatlBuyPolicy policy;
  policy.Init(fac, &inbuf, "inbuf", &buf_tracker, throughput, a_dist, d_dist, size_dist)
        .Set("commod1").Start();

  EXPECT_NO_THROW(sim.Run());

  QueryResult qr = sim.db().Query("Resources", NULL);
  EXPECT_NEAR(5.77748, qr.GetVal<double>("Quantity", 0), 0.00001);
  EXPECT_NEAR(3.17679, qr.GetVal<double>("Quantity", 1), 0.00001);

  QueryResult qr2 = sim.db().Query("Transactions", NULL);
  EXPECT_EQ(0, qr2.GetVal<int>("Time", 0));
  EXPECT_EQ(4, qr2.GetVal<int>("Time", 2));

  delete a;
}

TEST_F(MatlBuyPolicyTests, Cumulative_Cap_Inventory) {
  using cyclus::QueryResult;

  double ccap = 2;
  boost::shared_ptr<FixedIntDist> d_dist = boost::shared_ptr<FixedIntDist>(new FixedIntDist(2));

  int dur = 8;
  double throughput = 1;

  cyclus::MockSim sim(dur);
  cyclus::Agent* a = new TestFacility(sim.context());
  sim.context()->AddPrototype(a->prototype(), a);
  sim.agent = sim.context()->CreateAgent<cyclus::Agent>(a->prototype());
  sim.AddSource("commod1").Finalize();

  TestFacility* fac = dynamic_cast<TestFacility*>(sim.agent);

  cyclus::toolkit::ResBuf<cyclus::Material> inbuf;
  TotalInvTracker buf_tracker({&inbuf});
  cyclus::toolkit::MatlBuyPolicy policy;
  policy.Init(fac, &inbuf, "inbuf", &buf_tracker, throughput, ccap, d_dist)
        .Set("commod1").Start();

  EXPECT_NO_THROW(sim.Run());

  // check that transactions happen as expected (at time steps 0 and 1, then 4 and 5)
  QueryResult qr = sim.db().Query("Transactions", NULL);
  EXPECT_EQ(0, qr.GetVal<int>("Time", 0));
  EXPECT_EQ(1, qr.GetVal<int>("Time", 1));
  EXPECT_EQ(4, qr.GetVal<int>("Time", 2));
  EXPECT_EQ(5, qr.GetVal<int>("Time", 3));

  delete a;
}

}
}
