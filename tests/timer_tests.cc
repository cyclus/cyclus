#include <gtest/gtest.h>

#include "context.h"
#include "facility.h"
#include "greedy_preconditioner.h"
#include "greedy_solver.h"
#include "recorder.h"
#include "timer.h"
#include "sqlite_back.h"

static std::string const path = "testdb.sqlite";

class FileDeleter {
 public:
  FileDeleter(std::string path) {
    path_ = path;
  }

  ~FileDeleter() {
    remove(path_.c_str());
  }

 private:
  std::string path_;
};

class Dier : public cyclus::Facility {
 public:
  Dier(cyclus::Context* ctx) : cyclus::Facility(ctx) {}
  virtual ~Dier() {}

  virtual cyclus::Agent* Clone() { return new Dier(context()); }
  virtual void InitInv(cyclus::Inventories& inv) {}
  virtual cyclus::Inventories SnapshotInv() { return cyclus::Inventories(); }

  void Tick(int time) {
    context()->SchedDecom(this);
  }
  void Tock(int time) {};
};

class Termer : public cyclus::Facility {
 public:
  Termer(cyclus::Context* ctx) : cyclus::Facility(ctx) {}
  virtual ~Termer() {}

  virtual cyclus::Agent* Clone() { return new Dier(context()); }
  virtual void InitInv(cyclus::Inventories& inv) {}
  virtual cyclus::Inventories SnapshotInv() { return cyclus::Inventories(); }

  void Tick(int time) {
    context()->KillSim();
  };
  void Tock(int time) {};
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
  bool early = qr.GetVal<int>("EarlyTerm");
  bool end = qr.GetVal<int>("EndTime");

  EXPECT_TRUE(early);
  EXPECT_EQ(0, end);
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
