
#include <gtest/gtest.h>

#include "sqlite_back.h"
#include "recorder.h"
#include "context.h"
#include "timer.h"
#include "sim_init.h"
#include "facility.h"
#include "resource_buff.h"

static const char* dbpath = "testsiminit.sqlite";

namespace cy = cyclus;

class Inver : public cy::Facility {
 public:
  Inver(cy::Context* ctx) : cy::Facility(ctx), val1(0) {}
  virtual ~Inver() {}

  virtual cy::Agent* Clone() {
    Inver* i = new Inver(context());
    i->InitFrom(this);
    return i;
  }

  virtual void InitFrom(Inver* a) {
    cy::Facility::InitFrom(a);
    val1 = a->val1;
  };

  virtual void InitFrom(cy::QueryableBackend* b) {
    cy::Facility::InitFrom(b);
    cy::QueryResult qr = b->Query("Info", NULL);
    val1 = qr.GetVal<int>("val1");
  };

  virtual void Snapshot(cy::DbInit di) {
    di.NewDatum("Info")
      ->AddVal("val1", val1)
      ->Record();
  };

  virtual void Build(cy::Agent* parent) {
    cy::Facility::Build(parent);

    cy::Composition::Ptr c = context()->GetRecipe("recipe1");
    cy::Material::Ptr m1 = cy::Material::Create(this, 1, c);
    cy::Material::Ptr m2 = cy::Material::Create(this, 2, c);

    c = context()->GetRecipe("recipe2");
    cy::Material::Ptr m3 = cy::Material::Create(this, 3, c);

    buf1.Push(m1);
    buf2.Push(m2);
    buf2.Push(m3);
  };

  virtual void InitInv(cy::Inventories& inv) {
    buf1.PushAll(inv["buf1"]);
    buf2.PushAll(inv["buf2"]);
  };

  virtual cy::Inventories SnapshotInv() {
    cy::Inventories invs;
    invs["buf1"] = buf1.PopN(buf1.count());
    invs["buf2"] = buf2.PopN(buf2.count());
    return invs;
  }
  virtual void Tick(int t) {context()->Snapshot();};
  virtual void Tock(int t) { };

  cy::ResourceBuff buf1;
  cy::ResourceBuff buf2;
  int val1;
};

class SimInitTest : public ::testing::Test {
 protected:
  virtual void SetUp() {
    remove(dbpath);
    b = new cy::SqliteBack(dbpath);
    rec.RegisterBackend(b);
    ctx = new cy::Context(&ti, &rec);
    ctx->InitSim(cy::SimInfo(5));

    cy::CompMap v;
    v[922350000] = 1;
    v[922380000] = 2;
    ctx->AddRecipe("recipe1", cy::Composition::CreateFromMass(v));
    v[922380000] = 3;
    ctx->AddRecipe("recipe2", cy::Composition::CreateFromMass(v));

    // create initial prototypes
    Inver* a1 = new Inver(ctx);
    cy::DynamicModule::man_agents_["::Inver"] = a1->Clone();
    a1->spec("::Inver");
    a1->val1 = 23;
    Inver* a2 = new Inver(ctx);
    a2->spec("::Inver");
    a2->val1 = 26;

    // manually snap and add prototypes
    cy::SimInit::SnapAgent(a1);
    cy::SimInit::SnapAgent(a2);
    ctx->AddPrototype("proto1", a1);
    ctx->AddPrototype("proto2", a2);

    // sched 2 agents, build 2 agents
    ctx->SchedBuild(NULL, "proto1", 2);
    ctx->SchedBuild(NULL, "proto2", 3);
    a1->Clone()->Build(NULL);
    a2->Clone()->Build(NULL);

    cy::SimInit::Snapshot(ctx);
    rec.Flush();
  }

  virtual void TearDown() {
    rec.Close();
    delete ctx;
    delete b;
    remove(dbpath);
  }

  int transid(cy::Context* ctx) { return ctx->trans_id_; };
  cy::SimInfo siminfo(cy::Context* ctx) { return ctx->si_; };

  cy::Context* ctx;
  cy::Timer ti;
  cy::Recorder rec;
  cy::SqliteBack* b;
};

TEST_F(SimInitTest, InitNextIds) {
  cy::SimInit si;
  si.Init(&rec, b);
  cy::Context* init_ctx = si.context();

  EXPECT_EQ(transid(ctx), transid(init_ctx));
}

TEST_F(SimInitTest, InitRecipes) {
  cy::SimInit si;
  si.Init(&rec, b);
  cy::Context* init_ctx = si.context();

  cy::CompMap orig1 = ctx->GetRecipe("recipe1")->mass();
  cy::CompMap init1 = init_ctx->GetRecipe("recipe1")->mass();
  EXPECT_FLOAT_EQ(orig1[922350000], init1[922350000]);
  EXPECT_FLOAT_EQ(orig1[922380000], init1[922380000]);

  cy::CompMap orig2 = ctx->GetRecipe("recipe1")->mass();
  cy::CompMap init2 = init_ctx->GetRecipe("recipe1")->mass();
  EXPECT_FLOAT_EQ(orig2[922350000], init2[922350000]);
  EXPECT_FLOAT_EQ(orig2[922380000], init2[922380000]);
}

TEST_F(SimInitTest, InitSimInfo) {
  cy::SimInit si;
  si.Init(&rec, b);
  cy::Context* init_ctx = si.context();

  cy::SimInfo si_orig = ctx->sim_info();
  cy::SimInfo si_init = init_ctx->sim_info();

  EXPECT_EQ(ctx->time(), init_ctx->time());
  EXPECT_EQ(si_orig.duration, si_init.duration);
  EXPECT_EQ(si_orig.y0, si_init.y0);
  EXPECT_EQ(si_orig.m0, si_init.m0);
  EXPECT_EQ(si_orig.decay_period, si_init.decay_period);
  EXPECT_EQ(si_orig.handle, si_init.handle);
  EXPECT_EQ(si_orig.parent_sim, si_init.parent_sim);
  EXPECT_EQ(si_orig.parent_type, si_init.parent_type);
  EXPECT_EQ(si_orig.branch_time, si_init.branch_time);
}

TEST_F(SimInitTest, RestartSimInfo) {
  ti.RunSim();
  rec.Flush();
  cy::SimInit si;
  si.Restart(b, rec.sim_id(), 2);
  cy::Context* restart_ctx = si.context();

  cy::SimInfo si_orig = ctx->sim_info();
  cy::SimInfo si_restart = restart_ctx->sim_info();

  EXPECT_NE(rec.sim_id(), si.recorder()->sim_id());
  EXPECT_EQ(2, restart_ctx->time());
  EXPECT_EQ(si_orig.duration, si_restart.duration);
  EXPECT_EQ(si_orig.y0, si_restart.y0);
  EXPECT_EQ(si_orig.m0, si_restart.m0);
  EXPECT_EQ(si_orig.decay_period, si_restart.decay_period);
  EXPECT_EQ(si_orig.handle, si_restart.handle);
  EXPECT_EQ(rec.sim_id(), si_restart.parent_sim);
  EXPECT_EQ("restart", si_restart.parent_type);
  EXPECT_EQ(2, si_restart.branch_time);
}
