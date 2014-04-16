
#include <gtest/gtest.h>

#include "context.h"
#include "facility.h"
#include "greedy_preconditioner.h"
#include "greedy_solver.h"
#include "recorder.h"
#include "timer.h"

class Dier: public cyclus::Facility {
 public:
  Dier(cyclus::Context* ctx) : cyclus::Facility(ctx) {};
  virtual ~Dier() {};
  
  virtual cyclus::Agent* Clone() { return new Dier(context()); };
  virtual void InitInv(cyclus::Inventories& inv) {};
  virtual cyclus::Inventories SnapshotInv() {return cyclus::Inventories();};

  void Tick(int time) {
    context()->SchedDecom(this);
  };

  void Decommission() {
    cyclus::Facility::Decommission();
  }
  void Tock(int time) {};
};

TEST(TimerTests, NullParentDecomSegfault) {
  cyclus::Recorder rec;
  cyclus::Timer ti;
  cyclus::Context ctx(&ti, &rec);

  std::map<std::string, double> commod_order;
  cyclus::GreedyPreconditioner* conditioner = new cyclus::GreedyPreconditioner(
    commod_order,
    cyclus::GreedyPreconditioner::REVERSE);
  cyclus::GreedySolver* solver = new cyclus::GreedySolver(false, conditioner);
  ctx.solver(solver);

  Dier* d = new Dier(&ctx);
  d->Build(NULL);

  ti.Initialize(&ctx, cyclus::SimInfo(2));

  // EXPECT_NO_SEGFAULT
  ti.RunSim();
}
