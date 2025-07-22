#include "platform.h"
#if CYCLUS_IS_PARALLEL
#include <omp.h>
#endif // CYCLUS_IS_PARALLEL
#include <gtest/gtest.h>

#include "context.h"
#include "facility.h"
#include "greedy_preconditioner.h"
#include "greedy_solver.h"
#include "pyhooks.h"
#include "recorder.h"
#include "timer.h"
#include "sqlite_back.h"

#include "tools.h"

// special name to tell sqlite to use in-mem db
static std::string const path = ":memory:";

class Dier : public cyclus::Facility {
 public:
  Dier(cyclus::Context* ctx) : cyclus::Facility(ctx) {}
  virtual ~Dier() {}

  virtual cyclus::Agent* Clone() { return new Dier(context()); }
  virtual void InitInv(cyclus::Inventories& inv) {}
  virtual cyclus::Inventories SnapshotInv() { return cyclus::Inventories(); }
  virtual void Decommission() {
    decom_count++;
  }

  void Tick() {
    if (context()->time() == 0) {
      context()->SchedDecom(this);
    }
  }
  void Tock() {}
  void Decision() {}
  static int decom_count;
};

int Dier::decom_count = 0;

class Termer : public cyclus::Facility {
 public:
  Termer(cyclus::Context* ctx) : cyclus::Facility(ctx) {}
  virtual ~Termer() {}

  virtual cyclus::Agent* Clone() { return new Dier(context()); }
  virtual void InitInv(cyclus::Inventories& inv) {}
  virtual cyclus::Inventories SnapshotInv() { return cyclus::Inventories(); }

  void Tick() { context()->KillSim(); }
  void Tock() {}
  void Decision() {}
};

class SnapperTick : public cyclus::Facility {
 public:
  SnapperTick(cyclus::Context* ctx) : cyclus::Facility(ctx), snap(false) {}
  virtual ~SnapperTick() {}

  virtual cyclus::Agent* Clone() { return new Dier(context()); }
  virtual void InitInv(cyclus::Inventories& inv) {}
  virtual cyclus::Inventories SnapshotInv() { return cyclus::Inventories(); }

  void Tick() { if (snap && (context()->time() % 3 == 0)) {context()->Snapshot();} }
  void Tock() {}
  void Decision() {}
  bool snap;
};

class SnapperTock : public cyclus::Facility {
 public:
  SnapperTock(cyclus::Context* ctx) : cyclus::Facility(ctx), snap(false) {}
  virtual ~SnapperTock() {}

  virtual cyclus::Agent* Clone() { return new Dier(context()); }
  virtual void InitInv(cyclus::Inventories& inv) {}
  virtual cyclus::Inventories SnapshotInv() { return cyclus::Inventories(); }

  void Tick() {}
  void Tock() { if (snap && (context()->time() % 3 == 0)) {context()->Snapshot();} }
  void Decision() {}
  bool snap;
};

class SnapperDec : public cyclus::Facility {
 public:
  SnapperDec(cyclus::Context* ctx) : cyclus::Facility(ctx), snap(false) {}
  virtual ~SnapperDec() {}

  virtual cyclus::Agent* Clone() { return new Dier(context()); }
  virtual void InitInv(cyclus::Inventories& inv) {}
  virtual cyclus::Inventories SnapshotInv() { return cyclus::Inventories(); }

  void Tick() {}
  void Tock() {}
  void Decision() { if (snap && (context()->time() % 3 == 0)) {context()->Snapshot();} }
  bool snap;
};

class TimerTestsFixture : public ::testing::TestWithParam<int> {
  protected:
    #if CYCLUS_IS_PARALLEL
    virtual void SetUp() {
      int nthreads = GetParam();
      omp_set_num_threads(nthreads);
    }

    virtual void TearDown() {
      omp_set_num_threads(1);
    }
    #endif // CYCLUS_IS_PARALLEL
};

TEST_P(TimerTestsFixture, BareSim) {
  cyclus::PyStart();
  cyclus::Recorder rec;
  cyclus::Timer ti;
  cyclus::Context ctx(&ti, &rec);

  ti.Initialize(&ctx, cyclus::SimInfo(5));

  ASSERT_NO_THROW(ti.RunSim());
  cyclus::PyStop();
}

TEST_P(TimerTestsFixture, EarlyTermination) {
  cyclus::PyStart();
  cyclus::Recorder rec;
  cyclus::Timer ti;
  cyclus::Context ctx(&ti, &rec);
  cyclus::SqliteBack b(path);
  rec.RegisterBackend(&b);

  ti.Initialize(&ctx, cyclus::SimInfo(10));

  Termer* arnold = new Termer(&ctx);
  arnold->Build(NULL);

  ti.RunSim();
  rec.Close();

  cyclus::QueryResult qr = b.Query("Finish", NULL);
  bool early = qr.GetVal<bool>("EarlyTerm");
  int end = qr.GetVal<int>("EndTime");

  EXPECT_TRUE(early);
  EXPECT_EQ(0, end);
  cyclus::PyStop();
}

TEST_P(TimerTestsFixture, DefaultSnapshotTick) {
  cyclus::PyStart();
  cyclus::Recorder rec;
  cyclus::Timer ti;
  cyclus::Context ctx(&ti, &rec);
  cyclus::SqliteBack b(path);
  rec.RegisterBackend(&b);

  ti.Initialize(&ctx, cyclus::SimInfo(10));

  SnapperTick* turtle = new SnapperTick(&ctx);
  turtle->Build(NULL);

  ti.RunSim();
  rec.Close();

  cyclus::QueryResult qr = b.Query("Snapshots", NULL);
  EXPECT_EQ(1, qr.rows.size());
  EXPECT_EQ(10, qr.GetVal<int>("Time", 0));
  cyclus::PyStop();
}

TEST_P(TimerTestsFixture, DefaultSnapshotTock) {
  cyclus::PyStart();
  cyclus::Recorder rec;
  cyclus::Timer ti;
  cyclus::Context ctx(&ti, &rec);
  cyclus::SqliteBack b(path);
  rec.RegisterBackend(&b);

  ti.Initialize(&ctx, cyclus::SimInfo(10));

  SnapperTock* turtle = new SnapperTock(&ctx);
  turtle->Build(NULL);

  ti.RunSim();
  rec.Close();

  cyclus::QueryResult qr = b.Query("Snapshots", NULL);
  EXPECT_EQ(1, qr.rows.size());
  EXPECT_EQ(10, qr.GetVal<int>("Time", 0));
  cyclus::PyStop();
}


TEST_P(TimerTestsFixture, DefaultSnapshotDec) {
  cyclus::PyStart();
  cyclus::Recorder rec;
  cyclus::Timer ti;
  cyclus::Context ctx(&ti, &rec);
  cyclus::SqliteBack b(path);
  rec.RegisterBackend(&b);

  ti.Initialize(&ctx, cyclus::SimInfo(10));

  SnapperDec* turtle = new SnapperDec(&ctx);
  turtle->Build(NULL);

  ti.RunSim();
  rec.Close();

  cyclus::QueryResult qr = b.Query("Snapshots", NULL);
  EXPECT_EQ(1, qr.rows.size());
  EXPECT_EQ(10, qr.GetVal<int>("Time", 0));
  cyclus::PyStop();
}

TEST_P(TimerTestsFixture, CustomSnapshot) {
  cyclus::PyStart();
  cyclus::Recorder rec;
  cyclus::Timer ti;
  cyclus::Context ctx(&ti, &rec);
  cyclus::SqliteBack b(path);
  rec.RegisterBackend(&b);

  ti.Initialize(&ctx, cyclus::SimInfo(10));

  SnapperTick* turtle = new SnapperTick(&ctx);
  turtle->snap = true;
  turtle->Build(NULL);

  ti.RunSim();
  rec.Close();

  cyclus::QueryResult qr = b.Query("Snapshots", NULL);
  EXPECT_EQ(4, qr.rows.size());
  EXPECT_EQ(1, qr.GetVal<int>("Time", 0));
  EXPECT_EQ(4, qr.GetVal<int>("Time", 1));
  EXPECT_EQ(7, qr.GetVal<int>("Time", 2));
  EXPECT_EQ(10, qr.GetVal<int>("Time", 3));
  cyclus::PyStop();
}

TEST_P(TimerTestsFixture, NullParentDecomNoSegfault) {
  cyclus::PyStart();
  cyclus::Recorder rec;
  cyclus::Timer ti;
  cyclus::Context ctx(&ti, &rec);

  ti.Initialize(&ctx, cyclus::SimInfo(2));

  Dier* d = new Dier(&ctx);
  d->Build(NULL);

  // EXPECT_NO_SEGFAULT
  ti.RunSim();
  cyclus::PyStop();
}

TEST_P(TimerTestsFixture, DoubleDecom) {
  cyclus::PyStart();
  cyclus::Recorder rec;
  cyclus::Timer ti;
  cyclus::Context ctx(&ti, &rec);

  ti.Initialize(&ctx, cyclus::SimInfo(4));

  Dier* d = new Dier(&ctx);
  d->Build(NULL);
  ctx.SchedDecom(d, 0);

  Dier::decom_count = 0;
  ti.RunSim();
  EXPECT_EQ(1, Dier::decom_count);
  cyclus::PyStop();
}

#if CYCLUS_IS_PARALLEL
INSTANTIATE_TEST_CASE_P(TimerTestsParallel, TimerTestsFixture, ::testing::Values(1, 2, 3, 4));
#else
INSTANTIATE_TEST_CASE_P(TimerTests, TimerTestsFixture, ::testing::Values(1));
#endif // CYCLUS_IS_PARALLEL
