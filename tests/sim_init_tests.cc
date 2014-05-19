
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
using cy::Agent;

class Inver : public cy::Facility {
 public:
  Inver(cy::Context* ctx) : cy::Facility(ctx), val1(0) {}
  virtual ~Inver() {}

  virtual Agent* Clone() {
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

  virtual void Build(Agent* parent) {
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

Agent* ConstructInver(cy::Context* ctx) {
  return new Inver(ctx);
}

class SimInitTest : public ::testing::Test {
 protected:
  virtual void SetUp() {
    remove(dbpath);
    resetnextids();
    cy::DynamicModule::man_ctors_[":Inver:Inver"] = ConstructInver;

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
    a1->spec(":Inver:Inver");
    a1->val1 = 23;
    a1->prototype_ = "proto1";
    Inver* a2 = new Inver(ctx);
    a2->spec(":Inver:Inver");
    a2->val1 = 26;
    a2->prototype_ = "proto2";

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

  void resetnextids() {
    Agent::next_id_ = 0;
    cy::Resource::nextstate_id_ = 1;
    cy::Resource::nextobj_id_ = 1;
    cy::Composition::next_id_ = 1;
    cy::Product::next_state_ = 1;
  };
  int agentid() { return Agent::next_id_; };
  int stateid() { return cy::Resource::nextstate_id_; };
  int objid() { return cy::Resource::nextobj_id_; };
  int compid() { return cy::Composition::next_id_; };
  int prodid() { return cy::Product::next_state_; };
  int transid(cy::Context* ctx) { return ctx->trans_id_; };

  cy::SimInfo siminfo(cy::Context* ctx) { return ctx->si_; };
  std::set<Agent*> agent_list(cy::Context* ctx) { return ctx->agent_list_; };

  cy::Context* ctx;
  cy::Timer ti;
  cy::Recorder rec;
  cy::SqliteBack* b;
};

TEST_F(SimInitTest, InitNextIds) {
  // retrieve next ids from global static vars before overwriting them
  int agent_id = agentid();
  int rsrc_state_id = stateid();
  int rsrc_obj_id = objid();
  int comp_qual_id = compid();
  int prod_qual_id = prodid();

  resetnextids();
  ASSERT_EQ(0, agentid());
  ASSERT_EQ(1, stateid());
  ASSERT_EQ(1, objid());
  ASSERT_EQ(1, compid());
  ASSERT_EQ(1, prodid());

  cy::SimInit si;
  si.Init(&rec, b);
  cy::Context* init_ctx = si.context();

  EXPECT_EQ(transid(ctx), transid(init_ctx));
  EXPECT_EQ(agent_id, agentid());
  EXPECT_EQ(rsrc_state_id, stateid());
  EXPECT_EQ(rsrc_obj_id, objid());
  EXPECT_EQ(comp_qual_id, compid());
  EXPECT_EQ(prod_qual_id, prodid());
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

TEST_F(SimInitTest, InitProtos) {
  cy::SimInit si;
  si.Init(&rec, b);
  cy::Context* init_ctx = si.context();

  Inver* p1;
  ASSERT_NO_THROW(p1 = init_ctx->CreateAgent<Inver>("proto1"));
  EXPECT_EQ(23, p1->val1);
  EXPECT_EQ(0, p1->buf1.count());
  EXPECT_EQ(0, p1->buf2.count());
  EXPECT_EQ("proto1", p1->prototype());
  EXPECT_EQ(-1, p1->enter_time());
  EXPECT_EQ(":Inver:Inver", p1->spec());

  Inver* p2;
  ASSERT_NO_THROW(p2 = init_ctx->CreateAgent<Inver>("proto2"));
  EXPECT_EQ(26, p2->val1);
  EXPECT_EQ(0, p2->buf1.count());
  EXPECT_EQ(0, p2->buf2.count());
  EXPECT_EQ("proto2", p2->prototype());
}

TEST_F(SimInitTest, InitAgentState) {
  cy::SimInit si;
  si.Init(&rec, b);
  std::set<Agent*> agents = agent_list(ctx);
  std::set<Agent*> init_agents = agent_list(si.context());

  std::map<int, Agent*> byid;
  std::map<int, Agent*> init_byid;
  std::set<Agent*>::iterator it;
  for (it = agents.begin(); it != agents.end(); ++it) {
    byid[(*it)->id()] = *it;
  }
  for (it = init_agents.begin(); it != init_agents.end(); ++it) {
    init_byid[(*it)->id()] = *it;
  }

  ASSERT_EQ(4, byid.size()); // 2 deployed, 2 protos
  ASSERT_EQ(4, init_byid.size()); // 2 deployed, 2 protos

  std::map<int, Agent*>::iterator i;
  for (i = byid.begin(); i != byid.end(); ++i) {
    int id = i->first;
    Agent* agent = i->second;
    if (i == byid.begin()) {
      // skip the extra agent registered to DynamicModule for testing
      continue;
    } else if (agent->enter_time() == -1) {
      // skip prototypes
      continue;
    }


    bool was_loaded = init_byid.count(id) > 0;
    EXPECT_TRUE(was_loaded) << "agent id " << id << " not found in loaded agents list";
    if (!was_loaded) {
      continue;
    }

    Agent* init_agent = init_byid[id];
    EXPECT_EQ(agent->parent(), init_agent->parent());
    EXPECT_EQ(agent->lifetime(), init_agent->lifetime());
    EXPECT_EQ(agent->enter_time(), init_agent->enter_time());
    EXPECT_EQ(agent->kind(), init_agent->kind());
    EXPECT_EQ(agent->prototype(), init_agent->prototype());
    EXPECT_EQ(agent->spec(), init_agent->spec());
  }
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
