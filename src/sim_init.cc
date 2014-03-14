
#include "sim_init.h"

#include "greedy_preconditioner.h"
#include "greedy_solver.h"
#include "region_model.h"

namespace cyclus {

class Dummy : public RegionModel {
 public:
  Dummy(Context* ctx) : RegionModel(ctx) {};
  Dummy* Clone() { return NULL; };
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

void SimInit::Init(Recorder* r, QueryBackend* b) {
  rec_ = new Recorder(); // use dummy recorder to avoid re-recording
  InitBase(b, r->sim_id(), 0);
  ctx_->rec_ = r; // switch back before running sim
}

void SimInit::Restart(QueryBackend* b, boost::uuids::uuid sim_id, int t) {
  rec_ = new Recorder();
  InitBase(b, sim_id, t);
}

void SimInit::Branch(QueryBackend* b, boost::uuids::uuid prev_sim_id,
                           int t,
                           boost::uuids::uuid new_sim_id) {
  throw Error("simulation branching feature not implemented");
}

void SimInit::InitBase(QueryBackend* b, boost::uuids::uuid simid, int t) {
  rec_->set_dump_count(10000); // this recorder is "real" and gets bigger buf
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
  // snapshot all agent internal state
  std::set<Model*> mlist = ctx->model_list_;
  std::set<Model*>::iterator it;
  for (it = mlist.begin(); it != mlist.end(); ++it) {
    Model* m = *it;
    if (m->enter_time() != -1) {
      SimInit::SnapAgent(m);
    }
  }

  // snapshot all next ids
  ctx->NewDatum("NextIds")
  ->AddVal("Time", ctx->time())
  ->AddVal("Object", std::string("Agent"))
  ->AddVal("NextId", Model::next_id_)
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
  ->AddVal("Object", std::string("Resource"))
  ->AddVal("NextId", Resource::nextid_)
  ->Record();
  ctx->NewDatum("NextIds")
  ->AddVal("Time", ctx->time())
  ->AddVal("Object", std::string("GenericResource"))
  ->AddVal("NextId", GenericResource::next_state_)
  ->Record();
};

void SimInit::SnapAgent(Model* m) {
  // call manually without agent impl injected to keep all Model state in a
  // single, consolidated db table
  m->Model::Snapshot(DbInit(m, true));

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
      ->AddVal("ResourceId", inv[i]->id())
      ->Record();
    }
  }
}

void SimInit::LoadInfo() {
  QueryResult qr = b_->Query("Info", NULL);
  int dur = qr.GetVal<int>("Duration");
  int dec = qr.GetVal<int>("DecayInterval");
  int y0 = qr.GetVal<int>("InitialYear");
  int m0 = qr.GetVal<int>("InitialMonth");
  ctx_->InitSim(SimInfo(dur, y0, m0, dec));
}

void SimInit::LoadRecipes() {
  QueryResult qr = b_->Query("Recipes", NULL);

  for (int i = 0; i < qr.rows.size(); ++i) {
    std::string recipe = qr.GetVal<std::string>("Recipe", i);
    int stateid = qr.GetVal<int>("StateId", i);

    std::vector<Cond> conds;
    conds.push_back(Cond("StateId", "==", stateid));
    QueryResult qr = b_->Query("Compositions", &conds);
    CompMap m;
    for (int j = 0; j < qr.rows.size(); ++j) {
      int nuc = qr.GetVal<int>("NucId", j);
      double frac = qr.GetVal<double>("MassFrac", j);
      m[nuc] = frac;
    }
    Composition::Ptr comp = Composition::CreateFromMass(m);
    ctx_->AddRecipe(recipe, comp);
  }
}

void SimInit::LoadSolverInfo() {
  QueryResult qr = b_->Query("CommodPriority", NULL);

  std::map<std::string, double> commod_order;
  for (int i = 0; i < qr.rows.size(); ++i) {
    std::string commod = qr.GetVal<std::string>("Commodity", i);
    double order = qr.GetVal<double>("SolutionOrder", i);
    commod_order[commod] = order;
  }

  // solver will delete conditioner
  GreedyPreconditioner* conditioner = new GreedyPreconditioner(
    commod_order,
    GreedyPreconditioner::REVERSE);

  // context will delete solver
  bool exclusive_orders = false;
  GreedySolver* solver = new GreedySolver(exclusive_orders, conditioner);

  ctx_->solver(solver);
}

void SimInit::LoadPrototypes() {
  QueryResult qr = b_->Query("Prototypes", NULL);
  for (int i = 0; i < qr.rows.size(); ++i) {
    std::string proto = qr.GetVal<std::string>("Prototype", i);
    int agentid = qr.GetVal<int>("AgentId", i);
    std::string impl = qr.GetVal<std::string>("Implementation", i);

    Model* m = DynamicModule::Make(ctx_, impl);
    m->set_model_impl(impl);

    std::vector<Cond> conds;
    conds.push_back(Cond("SimTime", "==", t_));
    conds.push_back(Cond("AgentId", "==", agentid));
    CondInjector ci(b_, conds);
    PrefixInjector pi(&ci, "AgentState");

    // call manually without agent impl injected
    m->Model::InitFrom(&pi);

    pi = PrefixInjector(&ci, "AgentState" + impl);
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
  std::map<int, int> parentmap; // map<agentid, parentid>
  std::map<int, Model*> unbuilt; // map<agentid, agent_ptr>
  for (int i = 0; i < qentry.rows.size(); ++i) {

    int id = qentry.GetVal<int>("AgentId", i);
    std::vector<Cond> conds;
    conds.push_back(Cond("AgentId", "==", id));
    QueryResult qexit;
    try {
      qexit = b_->Query("AgentExit", &conds);
    } catch (std::exception err) { } // table doesn't exist (okay)
    // if the agent wasn't decommissioned before t_ create and init it
    if (qexit.rows.size() == 0) {
      std::string proto = qentry.GetVal<std::string>("Prototype", i);
      Model* m = ctx_->CreateModel<Model>(proto);

      // agent-kernel init
      m->id_ = id;
      m->set_model_impl(qentry.GetVal<std::string>("Implementation", i));
      m->enter_time_ = qentry.GetVal<int>("EnterTime", i);
      unbuilt[id] = m;
      parentmap[id] = qentry.GetVal<int>("ParentId", i);

      // agent-custom init
      conds.push_back(Cond("SimTime", "==", t_));
      CondInjector ci(b_, conds);
      PrefixInjector pi(&ci, "AgentState");
      m->Model::InitFrom(&pi);
      pi = PrefixInjector(&ci, "AgentState" + m->model_impl());
      m->InitFrom(&pi);
    }
  }

  // construct agent hierarchy starting at roots (no parent) down
  std::map<int, Model*>::iterator it = unbuilt.begin();
  while (unbuilt.size() > 0) {
    int id = it->first;
    Model* m = it->second;
    int parentid = parentmap[id];

    if (parentid == -1) { // root agent
      m->Connect(NULL);
      agents_[id] = m;
      ++it;
      unbuilt.erase(id);
      m->DoRegistration();
    } else if (agents_.count(parentid) > 0) { // parent is built
      m->Connect(agents_[parentid]);
      agents_[id] = m;
      ++it;
      unbuilt.erase(id);
      m->DoRegistration();
    } else { // parent not built yet
      ++it;
    }
    if (it == unbuilt.end()) {
      it = unbuilt.begin();
    }
  }
}

void SimInit::LoadInventories() {
  std::map<int, Model*>::iterator it;
  for (it = agents_.begin(); it != agents_.end(); ++it) {
    Model* m = it->second;
    std::vector<Cond> conds;
    conds.push_back(Cond("SimTime", "==", t_));
    conds.push_back(Cond("AgentId", "==", m->id()));
    QueryResult qr;
    try {
      qr = b_->Query("AgentStateInventories", &conds);
    } catch (std::exception err) {return;} // table doesn't exist (okay)

    Inventories invs;
    for (int i = 0; i < qr.rows.size(); ++i) {
      std::string inv_name = qr.GetVal<std::string>("InventoryName", i);
      int resid = qr.GetVal<int>("ResourceId", i);
      invs[inv_name].push_back(LoadResource(resid));
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
  } catch (std::exception err) {return;} // table doesn't exist (okay)

  for (int i = 0; i < qr.rows.size(); ++i) {
    int t = qr.GetVal<int>("BuildTime", i);
    int parentid = qr.GetVal<int>("ParentId", i);
    std::string proto = qr.GetVal<std::string>("Prototype", i);
    ctx_->SchedBuild(agents_[parentid], proto, t);
  }
}

void SimInit::LoadDecomSched() {
  std::vector<Cond> conds;
  conds.push_back(Cond("DecomTime", ">", t_));
  QueryResult qr;
  try {
    qr = b_->Query("DecomSchedule", &conds);
  } catch (std::exception err) {return;} // table doesn't exist (okay)

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
    std::string obj = qr.GetVal<std::string>("Object");
    if (obj == "Agent") {
      Model::next_id_ = qr.GetVal<int>("NextId", i);
    } else if (obj == "Transaction") {
      ctx_->trans_id_ = qr.GetVal<int>("NextId", i);
    } else if (obj == "Composition") {
      Composition::next_id_ = qr.GetVal<int>("NextId", i);
    } else if (obj == "Resource") {
      Resource::nextid_ = qr.GetVal<int>("NextId", i);
    } else if (obj == "GenericResource") {
      GenericResource::next_state_ = qr.GetVal<int>("NextId", i);
    } else {
      throw IOError("Unexpected value in NextIds table: " + obj);
    }
  }
}

Resource::Ptr SimInit::LoadResource(int resid) {
  std::vector<Cond> conds;
  conds.push_back(Cond("ResourceId", "==", resid));
  QueryResult qr = b_->Query("Resources", &conds);
  ResourceType type = qr.GetVal<ResourceType>("Type");

  if (type == Material::kType) {
    return LoadMaterial(resid);
  } else if (type == GenericResource::kType) {
    return LoadGenericResource(resid);
  }
  throw IOError("Invalid resource type in output database: " + type);
}

Resource::Ptr SimInit::LoadMaterial(int resid) {
  // get special material object state
  std::vector<Cond> conds;
  conds.push_back(Cond("ResourceId", "==", resid));
  conds.push_back(Cond("Time", "==", t_));
  QueryResult qr = b_->Query("MaterialInfo", &conds);
  int prev_decay = qr.GetVal<int>("PrevDecayTime");

  // get general resource object info
  conds.clear();
  conds.push_back(Cond("ResourceId", "==", resid));
  qr = b_->Query("Resources", &conds);
  double qty = qr.GetVal<double>("Quantity");
  int stateid = qr.GetVal<int>("StateId");

  // create the composition and material
  Composition::Ptr comp = LoadComposition(stateid);
  Model* dummy = new Dummy(ctx_);
  Material::Ptr mat = Material::Create(dummy, qty, comp);
  mat->id_ = resid;
  mat->prev_decay_time_ = prev_decay;

  return mat;
}

Composition::Ptr SimInit::LoadComposition(int stateid) {
  std::vector<Cond> conds;
  conds.push_back(Cond("StateId", "==", stateid));
  QueryResult qr = b_->Query("Compositions", &conds);
  CompMap cm;
  for (int i = 0; i < qr.rows.size(); ++i) {
    int nucid = qr.GetVal<int>("NucId", i);
    double mass_frac = qr.GetVal<double>("MassFrac", i);
    cm[nucid] = mass_frac;
  }
  return Composition::CreateFromMass(cm);
}

Resource::Ptr SimInit::LoadGenericResource(int resid) {
  // get general resource object info
  std::vector<Cond> conds;
  conds.push_back(Cond("ResourceId", "==", resid));
  QueryResult qr = b_->Query("Resources", &conds);
  double qty = qr.GetVal<double>("Quantity");
  int stateid = qr.GetVal<int>("StateId");

  // get special GenericResource internal state
  conds.clear();
  conds.push_back(Cond("StateId", "==", stateid));
  qr = b_->Query("GenericResources", &conds);
  std::string quality = qr.GetVal<std::string>("Quality");

  // set static quality-stateid map to have same vals as db
  GenericResource::stateids_[quality] = stateid;

  Model* dummy = new Dummy(ctx_);
  return GenericResource::Create(dummy, qty, quality);
}

} // namespace cyclus



