#include <gtest/gtest.h>

#include "comp_math.h"
#include "composition.h"
#include "context.h"
#include "facility.h"
#include "material.h"
#include "recorder.h"
#include "sim_init.h"
#include "sqlite_back.h"
#include "timer.h"
#include "toolkit/resource_buff.h"

// special name to tell sqlite to use in-mem db
static const char* dbpath = ":memory:";

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
  }

  virtual void InitFrom(cy::QueryableBackend* b) {
    cy::Facility::InitFrom(b);
    cy::QueryResult qr = b->Query("Info", NULL);
    val1 = qr.GetVal<int>("val1");
  }

  virtual void Snapshot(cy::DbInit di) {
    di.NewDatum("Info")
        ->AddVal("val1", val1)
        ->Record();
  }

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
  }

  virtual void InitInv(cy::Inventories& inv) {
    buf1.PushAll(inv["buf1"]);
    buf2.PushAll(inv["buf2"]);
  }

  virtual cy::Inventories SnapshotInv() {
    cy::Inventories invs;
    invs["buf1"] = buf1.PopN(buf1.count());
    invs["buf2"] = buf2.PopN(buf2.count());
    buf1.PushAll(invs["buf1"]);
    buf2.PushAll(invs["buf2"]);
    return invs;
  }
  virtual void Tick() { context()->Snapshot(); }
  virtual void Tock() {};

  cy::toolkit::ResourceBuff buf1;
  cy::toolkit::ResourceBuff buf2;
  int val1;
};

Agent* ConstructInver(cy::Context* ctx) {
  return new Inver(ctx);
}

class SimInitTest : public ::testing::Test {
 public:
  SimInitTest() : rec((unsigned int) 300) {}

 protected:
  virtual void SetUp() {
    resetnextids();
    cy::DynamicModule::man_ctors_[":Inver:Inver"] = ConstructInver;

    b = new cy::SqliteBack(dbpath);
    rec.RegisterBackend(b);
    ctx = new cy::Context(&ti, &rec);
    ctx->NewDatum("SolverInfo")
        ->AddVal("Solver", "greedy")
        ->AddVal("ExclusiveOrders", true)
        ->Record();
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
    ctx->AddPrototype("proto1", a1);
    ctx->AddPrototype("proto2", a2);

    // sched 2 agents, build 2 agents
    Agent* b1 = a1->Clone();
    Agent* b2 = a2->Clone();
    b1->Build(NULL);
    b2->Build(NULL);
    ctx->SchedDecom(b1, 1);
    ctx->SchedDecom(b2, 2);
    ctx->SchedBuild(NULL, "proto1", 2);
    ctx->SchedBuild(NULL, "proto2", 3);

    cy::SimInit::Snapshot(ctx);
    rec.Flush();
  }

  virtual void TearDown() {
    rec.Close();
    delete ctx;
    delete b;
  }

  void resetnextids() {
    Agent::next_id_ = 0;
    cy::Resource::nextstate_id_ = 1;
    cy::Resource::nextobj_id_ = 1;
    cy::Composition::next_id_ = 1;
    cy::Product::next_qualid_ = 1;
  }
  int agentid() { return Agent::next_id_; }
  int stateid() { return cy::Resource::nextstate_id_; }
  int objid() { return cy::Resource::nextobj_id_; }
  int compid() { return cy::Composition::next_id_; }
  int prodid() { return cy::Product::next_qualid_; }
  int transid(cy::Context* ctx) { return ctx->trans_id_; }

  cy::SimInfo siminfo(cy::Context* ctx) { return ctx->si_; }
  std::set<Agent*> agent_list(cy::Context* ctx) { return ctx->agent_list_; }
  std::map<int, cy::TimeListener*> tickers(cy::Timer* ti) { return ti->tickers_; }

  std::map<int, std::vector<std::pair<std::string, Agent*> > >
  build_queue(cy::Timer* ti) {
    return ti->build_queue_;
  }
  std::map<int, std::vector<Agent*> > decom_queue(cy::Timer* ti) {
    return ti->decom_queue_;
  }

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
  cy::compmath::Normalize(&orig1);
  cy::CompMap init1 = init_ctx->GetRecipe("recipe1")->mass();
  EXPECT_EQ(ctx->GetRecipe("recipe1")->id(), init_ctx->GetRecipe("recipe1")->id());
  EXPECT_FLOAT_EQ(orig1[922350000], init1[922350000]);
  EXPECT_FLOAT_EQ(orig1[922380000], init1[922380000]);

  cy::CompMap orig2 = ctx->GetRecipe("recipe1")->mass();
  cy::compmath::Normalize(&orig2);
  cy::CompMap init2 = init_ctx->GetRecipe("recipe1")->mass();
  EXPECT_EQ(ctx->GetRecipe("recipe2")->id(), init_ctx->GetRecipe("recipe2")->id());
  EXPECT_FLOAT_EQ(orig2[922350000], init2[922350000]);
  EXPECT_FLOAT_EQ(orig2[922380000], init2[922380000]);
}

TEST_F(SimInitTest, InitTimeListeners) {
  cy::SimInit si;
  si.Init(&rec, b);
  std::map<int, cy::TimeListener*> init_tickers = tickers(si.timer());

  ASSERT_EQ(2, init_tickers.size());
}

TEST_F(SimInitTest, InitBuildSched) {
  cy::SimInit si;
  si.Init(&rec, b);
  std::map<int, std::vector<std::pair<std::string, Agent*> > > queue = build_queue(si.timer());

  EXPECT_EQ(2, queue.size());

  int n_sched_t2 = queue[2].size();
  EXPECT_EQ(1, n_sched_t2);
  if (n_sched_t2 == 1) {
    EXPECT_EQ("proto1", queue[2][0].first);
  }

  int n_sched_t3 = queue[3].size();
  EXPECT_EQ(1, n_sched_t3);
  if (n_sched_t3 == 1) {
    EXPECT_EQ("proto2", queue[3][0].first);
  }
}

TEST_F(SimInitTest, InitDecomSched) {
  cy::SimInit si;
  si.Init(&rec, b);
  std::map<int, std::vector<Agent*> > queue = decom_queue(si.timer());

  EXPECT_EQ(2, queue.size());

  int n_sched_t1 = queue[1].size();
  EXPECT_EQ(1, n_sched_t1);
  if (n_sched_t1 == 1) {
    EXPECT_EQ(2, queue[1][0]->id());
  }

  int n_sched_t2 = queue[2].size();
  EXPECT_EQ(1, n_sched_t2);
  if (n_sched_t2 == 1) {
    EXPECT_EQ(3, queue[2][0]->id());
  }
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

  ASSERT_EQ(4, byid.size());  // 2 deployed, 2 protos
  ASSERT_EQ(4, init_byid.size());  // 2 deployed, 2 protos

  std::map<int, Agent*>::iterator i;
  for (i = byid.begin(); i != byid.end(); ++i) {
    int id = i->first;
    Agent* agent = i->second;
    if (agent->enter_time() == -1) {
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

TEST_F(SimInitTest, InitAgentInventories) {
  cy::SimInit si;
  si.Init(&rec, b);
  std::set<Agent*> init_agents = agent_list(si.context());
  std::set<Agent*> agents = agent_list(ctx);

  std::map<int, Agent*> byid;
  std::map<int, Agent*> init_byid;
  std::set<Agent*>::iterator it;
  for (it = agents.begin(); it != agents.end(); ++it) {
    byid[(*it)->id()] = *it;
  }
  for (it = init_agents.begin(); it != init_agents.end(); ++it) {
    init_byid[(*it)->id()] = *it;
  }

  ASSERT_EQ(4, init_byid.size());  // 2 deployed, 2 protos
  ASSERT_EQ(4, byid.size());  // 2 deployed, 2 protos

  cy::Composition::Ptr recipe1 = ctx->GetRecipe("recipe1");
  cy::Composition::Ptr recipe2 = ctx->GetRecipe("recipe2");

  std::map<int, Agent*>::iterator i;
  for (i = byid.begin(); i != byid.end(); ++i) {
    int id = i->first;
    Inver* agent = dynamic_cast<Inver*>(i->second);
    Inver* init_agent = dynamic_cast<Inver*>(init_byid[id]);
    if (agent->enter_time() == -1) {
      // skip prototypes
      continue;
    }

    EXPECT_EQ(1, agent->buf1.count());
    EXPECT_EQ(1, init_agent->buf1.count());
    EXPECT_EQ(2, agent->buf2.count());
    EXPECT_EQ(2, init_agent->buf2.count());

    // check agents' buf1 inventory
    cy::Material::Ptr mat1 = agent->buf1.Pop<cy::Material>();
    cy::Material::Ptr init_mat1 = init_agent->buf1.Pop<cy::Material>();

    EXPECT_EQ(mat1->qual_id(), init_mat1->qual_id());
    EXPECT_EQ(mat1->obj_id(), init_mat1->obj_id());
    EXPECT_EQ(mat1->state_id(), init_mat1->state_id());
    EXPECT_EQ(mat1->quantity(), init_mat1->quantity());

    // check agents' buf2 inventories
    mat1 = agent->buf2.Pop<cy::Material>();
    init_mat1 = init_agent->buf2.Pop<cy::Material>();
    cy::Material::Ptr mat2 = agent->buf2.Pop<cy::Material>();
    cy::Material::Ptr init_mat2 = init_agent->buf2.Pop<cy::Material>();

    EXPECT_EQ(mat1->qual_id(), init_mat1->qual_id());
    EXPECT_EQ(mat1->obj_id(), init_mat1->obj_id());
    EXPECT_EQ(mat1->state_id(), init_mat1->state_id());
    EXPECT_EQ(mat1->quantity(), init_mat1->quantity());

    EXPECT_EQ(mat2->qual_id(), init_mat2->qual_id());
    EXPECT_EQ(mat2->obj_id(), init_mat2->obj_id());
    EXPECT_EQ(mat2->state_id(), init_mat2->state_id());
    EXPECT_EQ(mat2->quantity(), init_mat2->quantity());
  }
}

TEST_F(SimInitTest, RestartSimInfo) {
  ti.RunSim();
  rec.Flush();
  cy::SimInit si;
  si.Restart(b, rec.sim_id(), 2);
  cy::SimInfo info = si.context()->sim_info();

  EXPECT_NE(rec.sim_id(), si.recorder()->sim_id());
  EXPECT_EQ(2, si.context()->time());
  EXPECT_EQ(5, info.duration);
  EXPECT_EQ(2010, info.y0);
  EXPECT_EQ(1, info.m0);
  EXPECT_EQ("", info.handle);
  EXPECT_EQ(rec.sim_id(), info.parent_sim);
  EXPECT_EQ("restart", info.parent_type);
  EXPECT_EQ(2, info.branch_time);
}
