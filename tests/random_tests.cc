#include <gtest/gtest.h>

#include "context.h"
#include "facility.h"
#include "pyhooks.h"
#include "recorder.h"
#include "sim_init.h"
#include "sqlite_back.h"
#include "timer.h"
#include "random_number_generator.h"

// special name to tell sqlite to use in-mem db
static const char* dbpath = ":memory:";

using cyclus::Agent;

class RandomInver : public cyclus::Facility {
 public:
  RandomInver(cyclus::Context* ctx) : cyclus::Facility(ctx), val1(0) {}
  virtual ~RandomInver() {}

  virtual Agent* Clone() {
    RandomInver* i = new RandomInver(context());
    i->InitFrom(this);
    return i;
  }

  virtual void InitFrom(RandomInver* a) {
    cyclus::Facility::InitFrom(a);
    val1 = a->val1;
  }

  virtual void InitFrom(cyclus::QueryableBackend* b) {
    cyclus::Facility::InitFrom(b);
    cyclus::QueryResult qr = b->Query("Info", NULL);
    val1 = qr.GetVal<int>("val1");
  }

  virtual void Snapshot(cyclus::DbInit di) {
    di.NewDatum("Info")
        ->AddVal("val1", val1)
        ->Record();
  }

  virtual void Build(Agent* parent) {
    cyclus::Facility::Build(parent);
  }

  virtual void InitInv(cyclus::Inventories& inv) {}

  virtual cyclus::Inventories SnapshotInv() {
    cyclus::Inventories invs;
    return invs;
  }
  virtual void Tick() { context()->Snapshot(); }
  virtual void Tock() {};

  int val1;
};

Agent* ConstructRandomInver(cyclus::Context* ctx) {
  return new RandomInver(ctx);
}

class RandomTest : public ::testing::Test {
 public:
  RandomTest() : rec((unsigned int) 300) {}

 protected:
  virtual void SetUp() {
    resetnextids();
    cyclus::DynamicModule::man_ctors_[":Inver:Inver"] = ConstructRandomInver;

    b = new cyclus::SqliteBack(dbpath);
    rec.RegisterBackend(b);
    ctx = new cyclus::Context(&ti, &rec);
    ctx->InitSim(cyclus::SimInfo(5));

    cyclus::SimInit::Snapshot(ctx);
    rec.Flush();
  }

  virtual void TearDown() {
    rec.Close();
    delete ctx;
    delete b;
  }

  void resetnextids() {
    Agent::next_id_ = 0;
    cyclus::Resource::nextstate_id_ = 1;
    cyclus::Resource::nextobj_id_ = 1;
    cyclus::Composition::next_id_ = 1;
    cyclus::Product::next_qualid_ = 1;
  }

  cyclus::Context* ctx;
  cyclus::Timer ti;
  cyclus::Recorder rec;
  cyclus::SqliteBack* b;
};

TEST_F(RandomTest, GetRandom) {
  std::uint32_t r = ctx->random();
  EXPECT_GE(r, 0);
  EXPECT_EQ(r, 1373056787);
  std::uint32_t r2 = ctx->random();
  EXPECT_GE(r2, 0);
  EXPECT_EQ(r2, 4042736447);
}

TEST_F(RandomTest, GetRandom01) {
  double r = ctx->random_01();
  EXPECT_GE(r, 0);
  EXPECT_LE(r, 1);
  EXPECT_NEAR(r, 0.31969, 0.00001);

}

TEST_F(RandomTest, GetRandomUniformInt) {
  int r = ctx->random_uniform_int(0, 10);
  EXPECT_GE(r, 0);
  EXPECT_LE(r, 10);
  EXPECT_EQ(r, 3);
}

TEST_F(RandomTest, GetRandomUniformReal) {
  double r = ctx->random_uniform_real(0, 10);
  EXPECT_GE(r, 0);
  EXPECT_LE(r, 10);
  EXPECT_NEAR(r, 3.1969, 0.00001);
}

TEST_F(RandomTest, GetRandomNormalReal) {
  double r = ctx->random_normal_real(5, 1, 0, 10);
  EXPECT_GE(r, 0);
  EXPECT_LE(r, 10);
  EXPECT_NEAR(r, 6.408382, 0.00001);
}

TEST_F(RandomTest, GetRandomNormalInt) {
  int r = ctx->random_normal_int(5, 1, 0, 10);
  EXPECT_GE(r, 0);
  EXPECT_LE(r, 10);
  EXPECT_EQ(r, 6);
}

TEST_F(RandomTest, TestRandomNormalFactory) {
  cyclus::NormalDist rn =
      cyclus::RandomNumberGenerator::makeNormalDist(5, 1, 0, 10);
  double r = rn();
  EXPECT_GE(r, 0);
  EXPECT_LE(r, 10);
  EXPECT_NEAR(r, 6.408382, 0.00001);
}