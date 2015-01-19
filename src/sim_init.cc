#include "sim_init.h"

#include "greedy_preconditioner.h"
#include "greedy_solver.h"
#include "portly_solver.h"
#include "region.h"

namespace cyclus {

class Dummy : public Region {
 public:
  Dummy(Context* ctx) : Region(ctx) {}
  Dummy* Clone() { return NULL; }
};

SimInit::SimInit() : rec_(NULL), ctx_(NULL) {}

SimInit::~SimInit() {
  if (ctx_ != NULL) {
    delete ctx_;
  }
  if (rec_ != NULL) {
    delete rec_;
  }
}

void SimInit::Init(Recorder* r, QueryableBackend* b) {
  rec_ = new Recorder();  // use dummy recorder to avoid re-recording
  InitBase(b, r->sim_id(), 0);
  ctx_->rec_ = r;  // switch back before running sim
}

void SimInit::Restart(QueryableBackend* b, boost::uuids::uuid sim_id, int t) {
  Warn<EXPERIMENTAL_WARNING>("restart capability is not finalized and fully"
                             " tested. Its behavior may change in future"
                             " releases.");
  rec_ = new Recorder();
  InitBase(b, sim_id, t);
  si_.parent_sim = sim_id;
  si_.parent_type = "restart";
  si_.branch_time = t;
  ctx_->InitSim(si_);  // explicitly force this to show up in the new simulations output db
}

void SimInit::Branch(QueryableBackend* b, boost::uuids::uuid prev_sim_id,
                     int t, boost::uuids::uuid new_sim_id) {
  throw Error("simulation branching feature not implemented");
}

void SimInit::InitBase(QueryableBackend* b, boost::uuids::uuid simid, int t) {
  ctx_ = new Context(&ti_, rec_);

  std::vector<Cond> conds;
  conds.push_back(Cond("SimId", "==", simid));
  b_ = new CondInjector(b, conds);
  t_ = t;
  simid_ = simid;

  // this sequence is imporant!!!
  LoadInfo();
  LoadRecipes();
  LoadSolverInfo();
  LoadPrototypes();
  LoadInitialAgents();
  LoadInventories();
  LoadBuildSched();
  LoadDecomSched();
  LoadNextIds();

  // delete all buffered data that we don't want to be re-recorded in the
  // output db
  rec_->Flush();
}

void SimInit::Snapshot(Context* ctx) {
  ctx->NewDatum("Snapshots")
     ->AddVal("Time", ctx->time())
     ->Record();

  // snapshot all agent internal state
  std::set<Agent*> mlist = ctx->agent_list_;
  std::set<Agent*>::iterator it;
  for (it = mlist.begin(); it != mlist.end(); ++it) {
    Agent* m = *it;
    if (m->enter_time() != -1) {
      SimInit::SnapAgent(m);
    }
  }

  // snapshot all next ids
  ctx->NewDatum("NextIds")
      ->AddVal("Time", ctx->time())
      ->AddVal("Object", std::string("Agent"))
      ->AddVal("NextId", Agent::next_id_)
      ->Record();
  ctx->NewDatum("NextIds")
      ->AddVal("Time", ctx->time())
      ->AddVal("Object", std::string("Transaction"))
      ->AddVal("NextId", ctx->trans_id_)
      ->Record();
  ctx->NewDatum("NextIds")
      ->AddVal("Time", ctx->time())
      ->AddVal("Object", std::string("Composition"))
      ->AddVal("NextId", Composition::next_id_)
      ->Record();
  ctx->NewDatum("NextIds")
      ->AddVal("Time", ctx->time())
      ->AddVal("Object", std::string("ResourceState"))
      ->AddVal("NextId", Resource::nextstate_id_)
      ->Record();
  ctx->NewDatum("NextIds")
      ->AddVal("Time", ctx->time())
      ->AddVal("Object", std::string("ResourceObj"))
      ->AddVal("NextId", Resource::nextobj_id_)
      ->Record();
  ctx->NewDatum("NextIds")
      ->AddVal("Time", ctx->time())
      ->AddVal("Object", std::string("Product"))
      ->AddVal("NextId", Product::next_qualid_)
      ->Record();
}

void SimInit::SnapAgent(Agent* m) {
  // call manually without agent impl injected to keep all Agent state in a
  // single, consolidated db table
  m->Agent::Snapshot(DbInit(m, true));

  m->Snapshot(DbInit(m));
  Inventories invs = m->SnapshotInv();
  Context* ctx = m->context();

  Inventories::iterator it;
  for (it = invs.begin(); it != invs.end(); ++it) {
    std::string name = it->first;
    std::vector<Resource::Ptr> inv = it->second;
    for (int i = 0; i < inv.size(); ++i) {
      ctx->NewDatum("AgentStateInventories")
          ->AddVal("AgentId", m->id())
          ->AddVal("SimTime", ctx->time())
          ->AddVal("InventoryName", name)
          ->AddVal("ResourceId", inv[i]->state_id())
          ->Record();
    }
  }
}

void SimInit::LoadInfo() {
  QueryResult qr = b_->Query("Info", NULL);
  int dur = qr.GetVal<int>("Duration");
  int y0 = qr.GetVal<int>("InitialYear");
  int m0 = qr.GetVal<int>("InitialMonth");
  std::string h = qr.GetVal<std::string>("Handle");

  QueryResult dq = b_->Query("DecayMode", NULL);
  std::string d = dq.GetVal<std::string>("Decay");
  si_ = SimInfo(dur, y0, m0, h, d);
  si_.parent_sim = qr.GetVal<boost::uuids::uuid>("ParentSimId");
  ctx_->InitSim(si_);
}

void SimInit::LoadRecipes() {
  QueryResult qr;
  try {
    qr = b_->Query("Recipes", NULL);
  } catch (std::exception err) {
    return;
  }  // table doesn't exist (okay)

  for (int i = 0; i < qr.rows.size(); ++i) {
    std::string recipe = qr.GetVal<std::string>("Recipe", i);
    int stateid = qr.GetVal<int>("QualId", i);
    Composition::Ptr c = LoadComposition(stateid);
    ctx_->AddRecipe(recipe, c);
  }
}

void* SimInit::LoadPreconditioner(std::string name) {
  using std::map;
  using std::string;
  void* precon = NULL;
  map<string, double> commod_order;
  try {
    QueryResult qr = b_->Query("CommodPriority", NULL);
    for (int i = 0; i < qr.rows.size(); ++i) {
      std::string commod = qr.GetVal<string>("Commodity", i);
      double order = qr.GetVal<double>("SolutionOrder", i);
      commod_order[commod] = order;
    }
  } catch (std::exception err) {
    return NULL;
  }  // table doesn't exist (okay)

  // actually create and return the preconditioner
  if (name == "greedy") {
    precon = new GreedyPreconditioner(commod_order, 
                                      GreedyPreconditioner::REVERSE);
  } else {
    throw ValueError("The name of the preconditioner was not recognized, "
                     "got '" + name + "'.");
  }
  return precon;
}

void SimInit::LoadSolverInfo() {
  using std::set;
  using std::string;
  // context will delete solver
  ExchangeSolver* solver;
  void* precon = NULL;
  string solver_name = string("greedy");
  string precon_name = string("greedy");
  bool exclusive_orders = false;

  // load in possible Solver info, needs to be optional to 
  // maintain backwards compatibility, defaults above.
  set<string> tables = b_->Tables();
  string solver_info = string("SolverInfo");
  if (0 < tables.count(solver_info)) {
    QueryResult qr = b_->Query(solver_info, NULL);
    if (qr.rows.size() > 0) {
      solver_name = qr.GetVal<string>("Solver");
      precon_name = qr.GetVal<string>("Preconditioner");
      exclusive_orders = qr.GetVal<bool>("ExclusiveOrders");
    }
  }

  precon = LoadPreconditioner(precon_name);

  if (solver_name == "greedy") {
    if (precon == NULL) {
      solver = new GreedySolver(exclusive_orders);
    } else {
      solver = new GreedySolver(exclusive_orders, 
        reinterpret_cast<GreedyPreconditioner*>(precon));
    }
  } else if (solver_name == "portly") {
      solver = new PortlySolver(exclusive_orders, precon);
  } else {
    throw ValueError("The name of the solver was not recognized, "
                     "got '" + solver_name + "'.");
  }
  
  ctx_->solver(solver);
}

void SimInit::LoadPrototypes() {
  QueryResult qr = b_->Query("Prototypes", NULL);
  for (int i = 0; i < qr.rows.size(); ++i) {
    std::string proto = qr.GetVal<std::string>("Prototype", i);
    int agentid = qr.GetVal<int>("AgentId", i);
    std::string impl = qr.GetVal<std::string>("Spec", i);
    AgentSpec spec(impl);

    Agent* m = DynamicModule::Make(ctx_, spec);
    m->id_ = agentid;

    // note that we don't filter by SimTime here because prototypes remain
    // static over the life of the simulation and we only snapshot them once
    // when the simulation is initialized.
    std::vector<Cond> conds;
    conds.push_back(Cond("AgentId", "==", agentid));
    CondInjector ci(b_, conds);
    PrefixInjector pi(&ci, "AgentState");

    // call manually without agent impl injected
    m->Agent::InitFrom(&pi);

    pi = PrefixInjector(&ci, "AgentState" + spec.Sanitize());
    m->InitFrom(&pi);
    ctx_->AddPrototype(proto, m);
  }
}

void SimInit::LoadInitialAgents() {
  // DO NOT call the agents' Build methods because the agents might modify the
  // state of their children and/or the simulation in ways that are only meant
  // to be done once; remember that we are initializing agents from a
  // simulation that was already started.

  // find all agents that are alive at the current timestep
  std::vector<Cond> conds;
  conds.push_back(Cond("EnterTime", "<=", t_));
  QueryResult qentry = b_->Query("AgentEntry", &conds);
  std::map<int, int> parentmap;  // map<agentid, parentid>
  std::map<int, Agent*> unbuilt;  // map<agentid, agent_ptr>
  for (int i = 0; i < qentry.rows.size(); ++i) {
    if (t_ > 0 && qentry.GetVal<int>("EnterTime", i) == t_) {
      // agent is scheduled to be built already
      continue;
    }
    int id = qentry.GetVal<int>("AgentId", i);
    std::vector<Cond> conds;
    conds.push_back(Cond("AgentId", "==", id));
    conds.push_back(Cond("ExitTime", "<", t_));
    try {
      QueryResult qexit = b_->Query("AgentExit", &conds);
      if (qexit.rows.size() != 0) {
        continue;  // agent was decomissioned before t_ - skip
      }
    } catch (std::exception err) {}  // table doesn't exist (okay)

    // if the agent wasn't decommissioned before t_ create and init it

    std::string proto = qentry.GetVal<std::string>("Prototype", i);
    std::string impl = qentry.GetVal<std::string>("Spec", i);
    AgentSpec spec(impl);
    Agent* m = DynamicModule::Make(ctx_, spec);

    // agent-kernel init
    m->prototype_ = proto;
    m->id_ = id;
    m->enter_time_ = qentry.GetVal<int>("EnterTime", i);
    unbuilt[id] = m;
    parentmap[id] = qentry.GetVal<int>("ParentId", i);

    // agent-custom init
    conds.pop_back();
    conds.push_back(Cond("SimTime", "==", t_));
    CondInjector ci(b_, conds);
    PrefixInjector pi(&ci, "AgentState");
    m->Agent::InitFrom(&pi);
    pi = PrefixInjector(&ci, "AgentState" + spec.Sanitize());
    m->InitFrom(&pi);
  }

  // construct agent hierarchy starting at roots (no parent) down
  std::map<int, Agent*>::iterator it = unbuilt.begin();
  std::vector<Agent*> enter_list;
  while (unbuilt.size() > 0) {
    int id = it->first;
    Agent* m = it->second;
    int parentid = parentmap[id];

    if (parentid == -1) {  // root agent
      m->Connect(NULL);
      agents_[id] = m;
      ++it;
      unbuilt.erase(id);
      enter_list.push_back(m);
    } else if (agents_.count(parentid) > 0) {  // parent is built
      m->Connect(agents_[parentid]);
      agents_[id] = m;
      ++it;
      unbuilt.erase(id);
      enter_list.push_back(m);
    } else {  // parent not built yet
      ++it;
    }
    if (it == unbuilt.end()) {
      it = unbuilt.begin();
    }
  }

  // notify all agents that they are active in a simulation AFTER the
  // parent-child hierarchy has been reconstructed.
  for (int i = 0; i < enter_list.size(); ++i) {
    enter_list[i]->EnterNotify();
  }
}

void SimInit::LoadInventories() {
  std::map<int, Agent*>::iterator it;
  for (it = agents_.begin(); it != agents_.end(); ++it) {
    Agent* m = it->second;
    std::vector<Cond> conds;
    conds.push_back(Cond("SimTime", "==", t_));
    conds.push_back(Cond("AgentId", "==", m->id()));
    QueryResult qr;
    try {
      qr = b_->Query("AgentStateInventories", &conds);
    } catch (std::exception err) {return;}  // table doesn't exist (okay)

    Inventories invs;
    for (int i = 0; i < qr.rows.size(); ++i) {
      std::string inv_name = qr.GetVal<std::string>("InventoryName", i);
      int state_id = qr.GetVal<int>("ResourceId", i);
      invs[inv_name].push_back(LoadResource(state_id));
    }
    m->InitInv(invs);
  }
}

void SimInit::LoadBuildSched() {
  std::vector<Cond> conds;
  conds.push_back(Cond("BuildTime", ">", t_));
  QueryResult qr;
  try {
    qr = b_->Query("BuildSchedule", &conds);
  } catch (std::exception err) {return;}  // table doesn't exist (okay)

  for (int i = 0; i < qr.rows.size(); ++i) {
    int t = qr.GetVal<int>("BuildTime", i);
    int parentid = qr.GetVal<int>("ParentId", i);
    std::string proto = qr.GetVal<std::string>("Prototype", i);
    ctx_->SchedBuild(agents_[parentid], proto, t);
  }
}

void SimInit::LoadDecomSched() {
  std::vector<Cond> conds;
  conds.push_back(Cond("DecomTime", ">=", t_));
  QueryResult qr;
  try {
    qr = b_->Query("DecomSchedule", &conds);
  } catch (std::exception err) {return;}  // table doesn't exist (okay)

  for (int i = 0; i < qr.rows.size(); ++i) {
    int t = qr.GetVal<int>("DecomTime", i);
    int agentid = qr.GetVal<int>("AgentId", i);
    ctx_->SchedDecom(agents_[agentid], t);
  }
}

void SimInit::LoadNextIds() {
  std::vector<Cond> conds;
  conds.push_back(Cond("Time", "==", t_));
  QueryResult qr = b_->Query("NextIds", &conds);
  for (int i = 0; i < qr.rows.size(); ++i) {
    std::string obj = qr.GetVal<std::string>("Object", i);
    if (obj == "Agent") {
      Agent::next_id_ = qr.GetVal<int>("NextId", i);
    } else if (obj == "Transaction") {
      ctx_->trans_id_ = qr.GetVal<int>("NextId", i);
    } else if (obj == "Composition") {
      Composition::next_id_ = qr.GetVal<int>("NextId", i);
    } else if (obj == "ResourceState") {
      Resource::nextstate_id_ = qr.GetVal<int>("NextId", i);
    } else if (obj == "ResourceObj") {
      Resource::nextobj_id_ = qr.GetVal<int>("NextId", i);
    } else if (obj == "Product") {
      Product::next_qualid_ = qr.GetVal<int>("NextId", i);
    } else {
      throw IOError("Unexpected value in NextIds table: " + obj);
    }
  }
}

Resource::Ptr SimInit::LoadResource(int state_id) {
  std::vector<Cond> conds;
  conds.push_back(Cond("ResourceId", "==", state_id));
  QueryResult qr = b_->Query("Resources", &conds);
  ResourceType type = qr.GetVal<ResourceType>("Type");
  int obj_id = qr.GetVal<int>("ObjId");

  Resource::Ptr r;
  if (type == Material::kType) {
    r = LoadMaterial(state_id);
  } else if (type == Product::kType) {
    r = LoadProduct(state_id);
  } else {
    throw IOError("Invalid resource type in output database: " + type);
  }

  r->state_id_ = state_id;
  r->obj_id_ = obj_id;
  return r;
}

Resource::Ptr SimInit::LoadMaterial(int state_id) {
  // get special material object state
  std::vector<Cond> conds;
  conds.push_back(Cond("ResourceId", "==", state_id));
  QueryResult qr = b_->Query("MaterialInfo", &conds);
  int prev_decay = qr.GetVal<int>("PrevDecayTime");

  // get general resource object info
  conds.clear();
  conds.push_back(Cond("ResourceId", "==", state_id));
  qr = b_->Query("Resources", &conds);
  double qty = qr.GetVal<double>("Quantity");
  int stateid = qr.GetVal<int>("QualId");

  // create the composition and material
  Composition::Ptr comp = LoadComposition(stateid);
  Agent* dummy = new Dummy(ctx_);
  Material::Ptr mat = Material::Create(dummy, qty, comp);
  mat->prev_decay_time_ = prev_decay;
  ctx_->DelAgent(dummy);

  return mat;
}

Composition::Ptr SimInit::LoadComposition(int stateid) {
  std::vector<Cond> conds;
  conds.push_back(Cond("QualId", "==", stateid));
  QueryResult qr = b_->Query("Compositions", &conds);
  CompMap cm;
  for (int i = 0; i < qr.rows.size(); ++i) {
    int nucid = qr.GetVal<int>("NucId", i);
    double mass_frac = qr.GetVal<double>("MassFrac", i);
    cm[nucid] = mass_frac;
  }
  Composition::Ptr c = Composition::CreateFromMass(cm);
  c->recorded_ = true;
  c->id_ = stateid;
  return c;
}

Resource::Ptr SimInit::LoadProduct(int state_id) {
  // get general resource object info
  std::vector<Cond> conds;
  conds.push_back(Cond("ResourceId", "==", state_id));
  QueryResult qr = b_->Query("Resources", &conds);
  double qty = qr.GetVal<double>("Quantity");
  int stateid = qr.GetVal<int>("QualId");

  // get special Product internal state
  conds.clear();
  conds.push_back(Cond("QualId", "==", stateid));
  qr = b_->Query("Products", &conds);
  std::string quality = qr.GetVal<std::string>("Quality");

  // set static quality-stateid map to have same vals as db
  Product::qualids_[quality] = stateid;

  Agent* dummy = new Dummy(ctx_);
  Resource::Ptr r = Product::Create(dummy, qty, quality);
  ctx_->DelAgent(dummy);
  return r;
}

}  // namespace cyclus
