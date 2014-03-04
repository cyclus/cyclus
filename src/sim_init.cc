
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
    int agentid = qr.GetVal<int>(i, "AgentId");
    std::string impl = qr.GetVal<std::string>(i, "Implementation");

    Model* m = DynamicModule::Make(se_->ctx, impl);
    m->set_model_impl(impl);

    std::vector<Cond> conds;
    conds.push_back(Cond("Time", "==", t_));
    conds.push_back(Cond("AgentId", "==", agentid));
    CondInjector ci(b_, conds);
    PrefixInjector pi(&ci, "AgentState_");
    m->InitFrom(&pi);
    se_->ctx->AddPrototype(proto, m);
  }
}

void SimInit::LoadInitialAgents() {
  // DO NOT call the agents' Build methods because the agents might modify the
  // state of their children and/or the simulation in ways that are only meant
  // to be done once; remember that we are initializing agents from a
  // simulation that was already started.

  // find the max agent id
  QueryResult qr = b_->Query("AgentEntry", NULL);
  int maxid = 0;
  for (int i = 0; i < qr.rows.size(); ++i) {
    int id = qr.GetVal<int>(i, "AgentId");
    if (id > maxid) {
      maxid = id;
    }
  }
  Model::next_id_ = maxid + 5;
  SHOW(maxid);

  // find all agents that are alive at the current timestep
  std::vector<Cond> conds;
  conds.push_back(Cond("EnterTime", "<=", t_));
  QueryResult qentry = b_->Query("AgentEntry", &conds);
  std::map<int, int> parentmap; // map<agentid, parentid>
  std::map<int, Model*> unbuilt; // map<agentid, agent_ptr>
  for (int i = 0; i < qentry.rows.size(); ++i) {

    int id = qentry.GetVal<int>(i, "AgentId");
    SHOW(id);
    std::vector<Cond> conds;
    conds.push_back(Cond("AgentId", "==", id));
    QueryResult qexit;
    try {
      qexit = b_->Query("AgentExit", &conds);
    } catch(std::exception err) { } // table doesn't exist (okay)
    // if the agent wasn't decommissioned before t_ create and init it
    if (qexit.rows.size() == 0) {
      std::string proto = qentry.GetVal<std::string>(i, "Prototype");
      Model* m = se_->ctx->CreateModel<Model>(proto);

      // agent-kernel init
      m->id_ = id;
      m->set_model_impl(qentry.GetVal<std::string>(i, "Implementation"));
      m->birthtime_ = qentry.GetVal<int>(i, "EnterTime");
      unbuilt[id] = m;
      parentmap[id] = qentry.GetVal<int>(i, "ParentId");

      // agent-custom init
      conds.push_back(Cond("Time", "==", t_));
      CondInjector ci(b_, conds);
      PrefixInjector pi(&ci, "AgentState_");
      m->InitFrom(&pi);

      SHOW(m->prototype());
      SHOW(m->model_impl_);
      SHOW(m->lifetime());
      SHOW(m->kind());
      SHOW(m->birthtime());
    }
  }

  // construct agent hierarchy starting at roots (no parent) down
  std::map<int, Model*>::iterator it = unbuilt.begin();
  while (unbuilt.size() > 0) {
    int id = it->first;
    Model* m = it->second;
    int parentid = parentmap[id];

    if (parentid == -1) { // root agent
      m->BuildInner(NULL);
      agents_[id] = m;
      ++it;
      unbuilt.erase(id);
    } else if (agents_.count(parentid) > 0) { // parent is built
      m->BuildInner(agents_[parentid]);
      agents_[id] = m;
      ++it;
      unbuilt.erase(id);
    } else { // parent not built yet
      ++it;
    }
    if (it == unbuilt.end()) {
      it = unbuilt.begin();
    }
  }
}

void SimInit::LoadInventories() {
}

void SimInit::LoadBuildSched() {
  std::vector<Cond> conds;
  conds.push_back(Cond("BuildTime", ">", t_));
  QueryResult qr;
  try {
    qr = b_->Query("BuildSchedule", &conds);
  } catch(std::exception err) { } // table doesn't exist (okay)

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
  QueryResult qr;
  try {
    qr = b_->Query("DecomSchedule", &conds);
  } catch(std::exception err) { } // table doesn't exist (okay)

  for (int i = 0; i < qr.rows.size(); ++i) {
    int t = qr.GetVal<int>(i, "DecomTime");
    int agentid = qr.GetVal<int>(i, "AgentId");
    se_->ctx->SchedDecom(agents_[agentid], t);
  }
}

} // namespace cyclus
