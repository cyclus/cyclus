#include "sim_init.h"

#include "greedy_preconditioner.h"
#include "greedy_solver.h"
#include "prog_solver.h"
#include "region.h"

namespace cyclus {

std::vector<Agent*> SimInit::agents() {
  std::vector<Agent*> ags;
  std::map<int, Agent*>::iterator it;
  for (it = agents_.begin(); it != agents_.end(); ++it) {
    ags.push_back(it->second);
  }
  return ags;
}

class Dummy : public Region {
 public:
  Dummy(Context* ctx) : Region(ctx) {}
  Dummy* Clone() { return NULL; }
};

SimInit::SimInit() : rec_(NULL), todel_rec_(NULL), ctx_(NULL) {}

SimInit::~SimInit() {
  if (ctx_ != NULL) {
    delete ctx_;
  }
  if (todel_rec_ != NULL) {
    delete todel_rec_;
  }
}

void SimInit::Init(Recorder* r, Context* src, int dur) {
  // DO NOT call the agents' Build methods because the agents might modify the
  // state of their children and/or the simulation in ways that are only meant
  // to be done once; remember that we are initializing agents from a
  // simulation that was already started.
  Warn<EXPERIMENTAL_WARNING>("init from context capability is not fully"
                             " tested");
  if (r == NULL) {
    rec_ = new Recorder();
    todel_rec_ = rec_;
  } else {
    rec_ = r;
  }

  si_ = src->sim_info();
  si_.parent_sim = src->sim_id();
  si_.parent_type = "context-clone";
  si_.branch_time = src->time();
  si_.duration = dur;

  Context* dst = new Context(&ti_, rec_);
  dst->solver(src->solver_);
  dst->trans_id_ = src->trans_id_;
  dst->recipes_ = src->recipes_;
  dst->n_prototypes_ = src->n_prototypes_;
  dst->n_specs_ = src->n_specs_;
  dst->InitSim(si_);
  ctx_ = dst;

  // all the prototypes need to be cloned into the new context
  std::map<std::string, Agent*>::iterator pit;
  for (pit = src->protos_.begin(); pit != src->protos_.end(); ++pit) {
    Agent* a = pit->second;
    a->ctx_ = dst;
    Agent* copy = a->Clone();
    copy->id_ = a->id_;
    dst->protos_[pit->first] = copy;
    a->ctx_ = src;
  }

  ////////// Clone and rebuild agent hierarchy ///////////
  std::set<Agent*>::iterator its;
  std::map<int, Agent*> unbuilt;  // map<agentid, agent_ptr>
  for (its = src->agent_list_.begin(); its != src->agent_list_.end(); ++its) {
    Agent* a = *its;
    if (a->enter_time() < 0) {
      // not a live agent
      continue;
    }

    a->ctx_ = dst;
    Agent* copy = a->Clone();
    a->ctx_ = src;

    // agent-kernel init
    copy->prototype_ = a->prototype_;
    copy->id_ = a->id_;
    copy->enter_time_ = a->enter_time_;
    copy->parent_id_ = a->parent_id_;
    unbuilt[copy->id()] = copy;
  }

  // construct agent hierarchy starting at roots (no parent) down
  std::map<int, Agent*>::iterator it = unbuilt.begin();
  std::vector<Agent*> enter_list;
  while (unbuilt.size() > 0) {
    int id = it->first;
    Agent* m = it->second;
    int parentid = m->parent_id_;

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

  ////////// clone and load all agent inventories /////////
  for (its = src->agent_list_.begin(); its != src->agent_list_.end(); ++its) {
    Agent* orig = *its;
    if (orig->enter_time() < 0) {
      // not a live agent
      continue;
    }

    Agent* a = agents_[orig->id()];
    Inventories invs = orig->SnapshotInv();
    Inventories copyinvs;
    Inventories::iterator invit;
    for (invit = invs.begin(); invit != invs.end(); ++invit) {
      std::string name = invit->first;
      std::vector<Resource::Ptr>& resvec = invit->second;
      std::vector<Resource::Ptr> copyvec;
      for (int i = 0; i < resvec.size(); i++) {
        copyvec.push_back(resvec[i]->Clone());
      }
      copyinvs[name] = copyvec;
    }
    a->InitInv(copyinvs);
  }

  ////////// clone build and decom schedules //////////////
  std::map<int, std::vector<std::pair<std::string, Agent*> > >::iterator bit;
  std::map<int, std::vector<std::pair<std::string, Agent*> > > builds = src->ti_->build_queue_;
  for (bit = builds.begin(); bit != builds.end(); ++bit) {
    int t = bit->first;
    if (t < context()->time()) {
      // skip builds in the past
      continue;
    }
    std::vector<std::pair<std::string, Agent*> > list = bit->second;
    for (int i = 0; i < list.size(); i++) {
      Agent* orig = list[i].second;
      if (orig != NULL) {
        Agent* a = agents_[orig->id_];
        dst->ti_->build_queue_[t].push_back(std::make_pair(list[i].first, a));
      } else {
        dst->ti_->build_queue_[t].push_back(std::make_pair(list[i].first, orig));
      }
    }
  }

  std::map<int, std::vector<Agent*> >::iterator dit;
  std::map<int, std::vector<Agent*> >& decoms = src->ti_->decom_queue_;
  for (dit = decoms.begin(); dit != decoms.end(); ++dit) {
    int t = dit->first;
    if (t < context()->time()) {
      // skip builds in the past
      continue;
    }
    std::vector<Agent*>& list = dit->second;
    for (int i = 0; i < list.size(); i++) {
      Agent* srca = list[i];
      int id = srca->id_;
      Agent* a = agents_[id];
      dst->ti_->decom_queue_[t].push_back(a);
    }
  }

}

void SimInit::Init(Recorder* r, QueryableBackend* b) {
  Recorder tmprec;
  rec_ = &tmprec;  // use dummy recorder to avoid re-recording
  InitBase(b, r->sim_id(), 0);
  ctx_->rec_ = r;  // switch back before running sim
}

void SimInit::Restart(QueryableBackend* b, boost::uuids::uuid sim_id, int t) {
  Warn<EXPERIMENTAL_WARNING>("restart capability is not finalized and fully"
                             " tested. Its behavior may change in future"
                             " releases.");
  rec_ = new Recorder();
  todel_rec_ = rec_;
  InitBase(b, sim_id, t);
  si_.parent_sim = sim_id;
  si_.parent_type = "restart";
  si_.branch_time = t;
  ctx_->InitSim(si_);  // explicitly force this to show up in the new simulations output db
}

void SimInit::Restart(QueryableBackend* b, boost::uuids::uuid sim_id, int t, int dur) {
  Warn<EXPERIMENTAL_WARNING>("restart capability is not finalized and fully"
                             " tested. Its behavior may change in future"
                             " releases.");
  rec_ = new Recorder();
  todel_rec_ = rec_;
  InitBase(b, sim_id, t);
  si_.parent_sim = sim_id;
  si_.parent_type = "restart";
  si_.branch_time = t;
  si_.duration = dur;
  ctx_->InitSim(si_);  // explicitly force this to show up in the new simulations output db
}

void SimInit::Branch(QueryableBackend* b, boost::uuids::uuid prev_sim_id,
                     int t, boost::uuids::uuid new_sim_id) {
  throw Error("simulation branching feature not implemented");
}

void SimInit::InitBase(QueryableBackend* b, boost::uuids::uuid simid, int t) {
  ctx_ = new Context(&ti_, rec_);
  ctx_->db_ = b;

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

  qr = b_->Query("TimeStepDur", NULL);
  // TODO: when the backends support uint64_t, the int template here
  // should be updated to uint64_t.
  si_.dt = qr.GetVal<int>("DurationSecs");

  qr = b_->Query("Epsilon", NULL);
  si_.eps = qr.GetVal<double>("GenericEpsilon");
  si_.eps_rsrc = qr.GetVal<double>("ResourceEpsilon");
  
  qr = b_->Query("InfoExplicitInv", NULL);
  si_.explicit_inventory = qr.GetVal<bool>("RecordInventory");
  si_.explicit_inventory_compact = qr.GetVal<bool>("RecordInventoryCompact");

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
    Composition::Ptr c = LoadComposition(b_, stateid);
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

ExchangeSolver* SimInit::LoadGreedySolver(bool exclusive, 
                                          std::set<std::string> tables) {
  using std::set;
  using std::string;
  ExchangeSolver* solver;
  void* precon = NULL;
  string precon_name = string("greedy");

  string solver_info = string("GreedySolverInfo");
  if (0 < tables.count(solver_info)) {
    QueryResult qr = b_->Query(solver_info, NULL);
    if (qr.rows.size() > 0) {
      precon_name = qr.GetVal<string>("Preconditioner");
    }
  }

  precon = LoadPreconditioner(precon_name);
  if (precon == NULL) {
    solver = new GreedySolver(exclusive);
  } else {
    solver = new GreedySolver(exclusive,
      reinterpret_cast<GreedyPreconditioner*>(precon));
  }
  return solver;
}

ExchangeSolver* SimInit::LoadCoinSolver(bool exclusive, 
                                        std::set<std::string> tables) {
  ExchangeSolver* solver;
  double timeout;
  bool verbose, mps;
  
  std::string solver_info = "CoinSolverInfo";
  if (0 < tables.count(solver_info)) {
    QueryResult qr = b_->Query(solver_info, NULL);
    timeout = qr.GetVal<double>("Timeout");
    verbose = qr.GetVal<bool>("Verbose");
    mps = qr.GetVal<bool>("Mps");
  }

  // set timeout to default if input value is non-positive
  timeout = timeout <= 0 ? ProgSolver::kDefaultTimeout : timeout;
  solver = new ProgSolver("cbc", timeout, exclusive, verbose, mps);
  return solver;
}

void SimInit::LoadSolverInfo() {
  using std::set;
  using std::string;
  // context will delete solver
  ExchangeSolver* solver;
  string solver_name;
  bool exclusive_orders;

  // load in possible Solver info, needs to be optional to
  // maintain backwards compatibility, defaults above.
  set<string> tables = b_->Tables();
  string solver_info = string("SolverInfo");
  if (0 < tables.count(solver_info)) {
    QueryResult qr = b_->Query(solver_info, NULL);
    if (qr.rows.size() > 0) {
      solver_name = qr.GetVal<string>("Solver");
      exclusive_orders = qr.GetVal<bool>("ExclusiveOrders");
    }
  }

  if (solver_name == "greedy") {
    solver = LoadGreedySolver(exclusive_orders, tables);
  } else if (solver_name == "coin-or") {
    solver = LoadCoinSolver(exclusive_orders, tables);
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
      invs[inv_name].push_back(LoadResource(ctx_, b_, state_id));
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

Material::Ptr SimInit::BuildMaterial(QueryableBackend* b, int resid) {
  Timer ti;
  Recorder rec;
  Context ctx(&ti, &rec);

  // manually make this "untracked" to prevent segfaulting and other such
  // terrors because the created context is destructed by SimInit at the end
  // of this function.
  Material::Ptr m = ResCast<Material>(SimInit::LoadResource(&ctx, b, resid));
  m->tracker_.DontTrack();
  m->ctx_ = NULL;
  return m;
}

Product::Ptr SimInit::BuildProduct(QueryableBackend* b, int resid) {
  Timer ti;
  Recorder rec;
  Context ctx(&ti, &rec);

  // manually make this "untracked" to prevent segfaulting and other such
  // terrors because the created context is destructed by SimInit at the end
  // of this function.
  Product::Ptr p = ResCast<Product>(SimInit::LoadResource(&ctx, b, resid));
  p->tracker_.DontTrack();
  p->ctx_ = NULL;
  return p;
}

Resource::Ptr SimInit::LoadResource(Context* ctx, QueryableBackend* b, int state_id) {
  std::vector<Cond> conds;
  conds.push_back(Cond("ResourceId", "==", state_id));
  QueryResult qr = b->Query("Resources", &conds);
  ResourceType type = qr.GetVal<ResourceType>("Type");
  int obj_id = qr.GetVal<int>("ObjId");

  Resource::Ptr r;
  if (type == Material::kType) {
    r = LoadMaterial(ctx, b, state_id);
  } else if (type == Product::kType) {
    r = LoadProduct(ctx, b, state_id);
  } else {
    throw IOError("Invalid resource type in output database: " + type);
  }

  r->state_id_ = state_id;
  r->obj_id_ = obj_id;
  return r;
}

Material::Ptr SimInit::LoadMaterial(Context* ctx, QueryableBackend* b, int state_id) {
  // get special material object state
  std::vector<Cond> conds;
  conds.push_back(Cond("ResourceId", "==", state_id));
  QueryResult qr = b->Query("MaterialInfo", &conds);
  int prev_decay = qr.GetVal<int>("PrevDecayTime");

  // get general resource object info
  conds.clear();
  conds.push_back(Cond("ResourceId", "==", state_id));
  qr = b->Query("Resources", &conds);
  double qty = qr.GetVal<double>("Quantity");
  int stateid = qr.GetVal<int>("QualId");

  // create the composition and material
  Composition::Ptr comp = LoadComposition(b, stateid);
  Agent* dummy = new Dummy(ctx);
  Material::Ptr mat = Material::Create(dummy, qty, comp);
  mat->prev_decay_time_ = prev_decay;
  ctx->DelAgent(dummy);

  return mat;
}

Composition::Ptr SimInit::LoadComposition(QueryableBackend* b, int stateid) {
  std::vector<Cond> conds;
  conds.push_back(Cond("QualId", "==", stateid));
  QueryResult qr = b->Query("Compositions", &conds);
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

Product::Ptr SimInit::LoadProduct(Context* ctx, QueryableBackend* b, int state_id) {
  // get general resource object info
  std::vector<Cond> conds;
  conds.push_back(Cond("ResourceId", "==", state_id));
  QueryResult qr = b->Query("Resources", &conds);
  double qty = qr.GetVal<double>("Quantity");
  int stateid = qr.GetVal<int>("QualId");

  // get special Product internal state
  conds.clear();
  conds.push_back(Cond("QualId", "==", stateid));
  qr = b->Query("Products", &conds);
  std::string quality = qr.GetVal<std::string>("Quality");

  // set static quality-stateid map to have same vals as db
  Product::qualids_[quality] = stateid;

  Agent* dummy = new Dummy(ctx);
  Product::Ptr r = Product::Create(dummy, qty, quality);
  ctx->DelAgent(dummy);
  return r;
}

}  // namespace cyclus
