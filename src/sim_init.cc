
#include "sim_init.h"

#include "greedy_preconditioner.h"
#include "greedy_solver.h"

#define SHOW(X) \
  std::cout << __FILE__ << ":" << __LINE__ << ": "#X" = " << X << "\n"

namespace cyclus {

SimInit::SimInit() {
  se_ = new SimEngine();
  se_->ti = new Timer();
}


SimEngine* SimInit::Init(QueryBackend* b, boost::uuids::uuid simid) {
  se_->rec = new Recorder(simid);
  return InitBase(b, simid, 0);
}

SimEngine* SimInit::Restart(QueryBackend* b, boost::uuids::uuid simid, int t) {
  se_->rec = new Recorder();
  return InitBase(b, simid, t);
}

SimEngine* SimInit::InitBase(QueryBackend* b, boost::uuids::uuid simid, int t) {
  std::vector<Cond> conds;
  conds.push_back(Cond("SimId", "==", simid));
  b_ = new CondInjector(b, conds);
  t_ = t;
  simid_ = simid;

  se_->ctx = new Context(se_->ti, se_->rec);

  LoadControlParams();
  LoadRecipes();
  LoadSolverInfo();
  LoadPrototypes();
  LoadInitialAgents();
  LoadInventories();
  LoadBuildSched();
  LoadDecomSched();

  // use rec.set_dump_count to reset all buffered datums that we don't want
  // to be eventually sent to backends that are to-be-added.
  se_->rec->set_dump_count(kDefaultDumpCount);

  return se_;
}

void SimInit::LoadControlParams() {
  QueryResult qr = b_->Query("Info", NULL);

  int dur = qr.GetVal<int>(0, "Duration");
  int dec = qr.GetVal<int>(0, "DecayInterval");
  int y0 = qr.GetVal<int>(0, "InitialYear");
  int m0 = qr.GetVal<int>(0, "InitialMonth");

  se_->ctx->InitTime(dur, dec, m0, y0);
}

void SimInit::LoadRecipes() {
  QueryResult qr = b_->Query("Recipes", NULL);

  for (int i = 0; i < qr.rows.size(); ++i) {
    std::string recipe = qr.GetVal<std::string>(i, "Recipe");
    int stateid = qr.GetVal<int>(i, "StateId");

    std::vector<Cond> conds;
    conds.push_back(Cond("StateId", "==", stateid));
    QueryResult qr = b_->Query("Compositions", &conds);
    CompMap m;
    for (int j = 0; j < qr.rows.size(); ++j) {
      int nuc = qr.GetVal<int>(j, "NucId");
      double frac = qr.GetVal<double>(j, "MassFrac");
      m[nuc] = frac;
    }
    Composition::Ptr comp = Composition::CreateFromMass(m);
    se_->ctx->AddRecipe(recipe, comp);
  }
}

void SimInit::LoadSolverInfo() {
  QueryResult qr = b_->Query("CommodPriority", NULL);

  std::map<std::string, double> commod_order;
  for (int i = 0; i < qr.rows.size(); ++i) {
    std::string commod = qr.GetVal<std::string>(i, "Commodity");
    double order = qr.GetVal<double>(i, "SolutionOrder");
    commod_order[commod] = order;
  }

  // solver will delete conditioner
  GreedyPreconditioner* conditioner = new GreedyPreconditioner(
    commod_order,
    GreedyPreconditioner::REVERSE);

  // context will delete solver
  bool exclusive_orders = false;
  GreedySolver* solver = new GreedySolver(exclusive_orders, conditioner);

  se_->ctx->solver(solver);
}

void SimInit::LoadPrototypes() {
  QueryResult qr = b_->Query("Prototypes", NULL);
  for (int i = 0; i < qr.rows.size(); ++i) {
    std::string proto = qr.GetVal<std::string>(i, "Prototype");
    std::string impl = qr.GetVal<std::string>(i, "Implementation");

    Model* m = DynamicModule::Make(se_->ctx, impl);
    m->InitFrom(b_);
    se_->ctx->AddPrototype(proto, m);
  }
}

struct AgentInfo {
  int id;
  std::string impl;
  std::string proto;
  int entry;
  int parent;
};

void SimInit::LoadInitialAgents() {
  // DO NOT call the agents' Build methods because the agents might modify the
  // state of their children and/or the simulation in ways that are only meant
  // to be done once; remember that we are initializing agents from a
  // simulation that was already started.

  // find all agents that are alive at the current timestep
  std::vector<Cond> conds;
  conds.push_back(Cond("EnterTime", "<=", t_));
  QueryResult qentry = b_->Query("AgentEntry", &conds);
  std::vector<AgentInfo> infos;
  for (int i = 0; i < qentry.rows.size(); ++i) {

    // if the agent wasn't decommissioned before t_
    int id = qentry.GetVal<int>(i, "AgentId");
    std::vector<Cond> conds;
    conds.push_back(Cond("AgentId", "==", id));
    QueryResult qexit = b_->Query("AgentExit", &conds);
    if (qexit.rows.size() == 0) {
      AgentInfo ai;
      ai.id = id;
      ai.impl = qentry.GetVal<std::string>(i, "Implementation");
      ai.proto = qentry.GetVal<std::string>(i, "Prototype");
      ai.entry = qentry.GetVal<int>(i, "EnterTime");
      ai.parent = qentry.GetVal<int>(i, "ParentId");
      infos.push_back(ai);
    }
  }


}

void SimInit::LoadInventories() {
}

void SimInit::LoadBuildSched() {
  std::vector<Cond> conds;
  conds.push_back(Cond("BuildTime", ">", t_));
  QueryResult qr = b_->Query("BuildSchedule", &conds);

  for (int i = 0; i < qr.rows.size(); ++i) {
    int t = qr.GetVal<int>(i, "BuildTime");
    int parentid = qr.GetVal<int>(i, "ParentId");
    std::string proto = qr.GetVal<std::string>(i, "Prototype");
    se_->ctx->SchedBuild(agents_[parentid], proto, t);
  }
}

void SimInit::LoadDecomSched() {
  std::vector<Cond> conds;
  conds.push_back(Cond("DecomTime", ">", t_));
  QueryResult qr = b_->Query("DecomSchedule", &conds);

  for (int i = 0; i < qr.rows.size(); ++i) {
    int t = qr.GetVal<int>(i, "DecomTime");
    int agentid = qr.GetVal<int>(i, "AgentId");
    se_->ctx->SchedDecom(agents_[agentid], t);
  }
}

} // namespace cyclus
