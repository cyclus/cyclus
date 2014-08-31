#include <gtest/gtest.h>

#include "context.h"
#include "facility.h"
#include "greedy_preconditioner.h"
#include "greedy_solver.h"
#include "recorder.h"
#include "timer.h"
#include "sqlite_back.h"

#include "tools.h"

static std::string const path = "testdb.sqlite";

class Dier : public cyclus::Facility {
 public:
  Dier(cyclus::Context* ctx) : cyclus::Facility(ctx) {}
  virtual ~Dier() {}

  virtual cyclus::Agent* Clone() { return new Dier(context()); }
  virtual void InitInv(cyclus::Inventories& inv) {}
  virtual cyclus::Inventories SnapshotInv() { return cyclus::Inventories(); }

  void Tick() { context()->SchedDecom(this); }
  void Tock() {}
};

class Termer : public cyclus::Facility {
 public:
  Termer(cyclus::Context* ctx) : cyclus::Facility(ctx) {}
  virtual ~Termer() {}

  virtual cyclus::Agent* Clone() { return new Dier(context()); }
  virtual void InitInv(cyclus::Inventories& inv) {}
  virtual cyclus::Inventories SnapshotInv() { return cyclus::Inventories(); }

  void Tick() { context()->KillSim(); }
  void Tock() {}
};

class Snapper : public cyclus::Facility {
 public:
  Snapper(cyclus::Context* ctx) : cyclus::Facility(ctx), snap(false) {}
  virtual ~Snapper() {}

  virtual cyclus::Agent* Clone() { return new Dier(context()); }
  virtual void InitInv(cyclus::Inventories& inv) {}
  virtual cyclus::Inventories SnapshotInv() { return cyclus::Inventories(); }

  void Tick() { if (snap && (context()->time() % 3 == 0)) {context()->Snapshot();} }
  void Tock() {}
  bool snap;
};

TEST(TimerTests, BareSim) {
  cyclus::Recorder rec;
  cyclus::Timer ti;
  cyclus::Context ctx(&ti, &rec);

  ti.Initialize(&ctx, cyclus::SimInfo(5));

  ASSERT_NO_THROW(ti.RunSim());
}

TEST(TimerTests, EarlyTermination) {
  FileDeleter fd(path);

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
}

TEST(TimerTests, DefaultSnapshot) {
  FileDeleter fd(path);

  cyclus::Recorder rec;
  cyclus::Timer ti;
  cyclus::Context ctx(&ti, &rec);
  cyclus::SqliteBack b(path);
  rec.RegisterBackend(&b);

  ti.Initialize(&ctx, cyclus::SimInfo(10));

  Snapper* turtle = new Snapper(&ctx);
  turtle->Build(NULL);

  ti.RunSim();
  rec.Close();

  cyclus::QueryResult qr = b.Query("Snapshots", NULL);
  EXPECT_EQ(1, qr.rows.size());
  EXPECT_EQ(10, qr.GetVal<int>("Time", 0));
}

TEST(TimerTests, CustomSnapshot) {
  FileDeleter fd(path);

  cyclus::Recorder rec;
  cyclus::Timer ti;
  cyclus::Context ctx(&ti, &rec);
  cyclus::SqliteBack b(path);
  rec.RegisterBackend(&b);

  ti.Initialize(&ctx, cyclus::SimInfo(10));

  Snapper* turtle = new Snapper(&ctx);
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
}

TEST(TimerTests, NullParentDecomSegfault) {
  cyclus::Recorder rec;
  cyclus::Timer ti;
  cyclus::Context ctx(&ti, &rec);

  ti.Initialize(&ctx, cyclus::SimInfo(2));

  Dier* d = new Dier(&ctx);
  d->Build(NULL);

  // EXPECT_NO_SEGFAULT
  ti.RunSim();
}
