
#include <gtest/gtest.h>
#include "cyclus.h"

class MultiTraderSink: public cyclus::Facility {
 public:
  static std::string proto_name() { return "test_fac_prototype"; }
  static std::string spec() { return "test_fac_impl"; }

  MultiTraderSink(cyclus::Context* ctx) : cyclus::Facility(ctx) {
    cyclus::Agent::prototype(proto_name());
    cyclus::Agent::spec(spec());
    inbuf1.capacity(5);
    inbuf2.capacity(5);
  }
  virtual ~MultiTraderSink() {}

  virtual cyclus::Agent* Clone() { return new MultiTraderSink(context()); }
  virtual void InitInv(cyclus::Inventories& inv) {}
  virtual cyclus::Inventories SnapshotInv() {return cyclus::Inventories();}


  void EnterNotify() {
    cyclus::Facility::EnterNotify();
    buff_tracker.Init({&inbuf1, &inbuf2});
    policy1.Init(this, &inbuf1, "inbuf1", &buff_tracker).Set("commod1").Start();
    policy2.Init(this, &inbuf2, "inbuf2", &buff_tracker).Set("commod2").Start();
  }

  void Tick() {}
  void Tock() {}

  cyclus::toolkit::MatlBuyPolicy policy1;
  cyclus::toolkit::ResBuf<cyclus::Material> inbuf1;
  cyclus::toolkit::MatlBuyPolicy policy2;
  cyclus::toolkit::ResBuf<cyclus::Material> inbuf2;
  cyclus::toolkit::TotalInvTracker buff_tracker;
};

// issue 1210 was preventing the 1209 test from passing (causing segfaults),
// so these issues are both covered by this test.  Checks that multiple
// traders from a single agent can successfully trade.
TEST(Bugfixes, Issue1209And1210) {
  int dur = 10;
  cyclus::MockSim sim(dur);
  cyclus::Agent* a = new MultiTraderSink(sim.context());
  sim.context()->AddPrototype(a->prototype(), a);
  sim.agent = sim.context()->CreateAgent<cyclus::Agent>(a->prototype());
  sim.AddSource("commod1").Finalize();
  sim.AddSource("commod2").Finalize();

  MultiTraderSink* fac = dynamic_cast<MultiTraderSink*>(sim.agent);

  EXPECT_NO_THROW(sim.Run());
  EXPECT_DOUBLE_EQ(5, fac->inbuf1.quantity());
  EXPECT_DOUBLE_EQ(5, fac->inbuf2.quantity());
}

