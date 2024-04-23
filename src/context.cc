#include "platform.h"
#include "context.h"

#include <vector>
#include <boost/uuid/uuid_generators.hpp>
#if CYCLUS_IS_PARALLEL
#include <omp.h>
#endif // CYCLUS_IS_PARALLEL

#include "error.h"
#include "exchange_solver.h"
#include "logger.h"
#include "pyhooks.h"
#include "sim_init.h"
#include "timer.h"
#include "random_number_generator.h"
#include "version.h"

namespace cyclus {

double cy_eps = 1e-6;
double cy_eps_rsrc = 1e-6;

SimInfo::SimInfo()
    : duration(0),
      y0(0),
      m0(0),
      dt(kDefaultTimeStepDur),
      decay("manual"),
      branch_time(-1),
      explicit_inventory(false),
      explicit_inventory_compact(false),
      parent_sim(boost::uuids::nil_uuid()),
      parent_type("init"),
      seed(kDefaultSeed),
      stride(kDefaultStride) {}

SimInfo::SimInfo(int dur, int y0, int m0, std::string handle)
    : duration(dur),
      y0(y0),
      m0(m0),
      dt(kDefaultTimeStepDur),
      decay("manual"),
      branch_time(-1),
      handle(handle),
      explicit_inventory(false),
      explicit_inventory_compact(false),
      parent_sim(boost::uuids::nil_uuid()),
      parent_type("init"),
      seed(kDefaultSeed),
      stride(kDefaultStride) {}

SimInfo::SimInfo(int dur, int y0, int m0, std::string handle, std::string d)
    : duration(dur),
      y0(y0),
      m0(m0),
      dt(kDefaultTimeStepDur),
      decay(d),
      branch_time(-1),
      handle(handle),
      explicit_inventory(false),
      explicit_inventory_compact(false),
      parent_sim(boost::uuids::nil_uuid()),
      parent_type("init"),
      seed(kDefaultSeed),
      stride(kDefaultStride) {}

SimInfo::SimInfo(int dur, boost::uuids::uuid parent_sim,
                 int branch_time, std::string parent_type,
                 std::string handle)
    : duration(dur),
      y0(-1),
      m0(-1),
      dt(kDefaultTimeStepDur),
      decay("manual"),
      parent_sim(parent_sim),
      parent_type(parent_type),
      branch_time(branch_time),
      explicit_inventory(false),
      explicit_inventory_compact(false),
      handle(handle),
      seed(kDefaultSeed),
      stride(kDefaultStride) {}

Context::Context(Timer* ti, Recorder* rec)
    : ti_(ti),
      rec_(rec),
      solver_(NULL),
      trans_id_(0),
      si_(0) {

        rng_ = new RandomNumberGenerator();
      }

Context::~Context() {
  if (solver_ != NULL) {
    delete solver_;
  }
  if (rng_ != NULL) {
    delete rng_;
  }
  // initiate deletion of agents that don't have parents.
  // dealloc will propagate through hierarchy as agents delete their children
  std::vector<Agent*> to_del;
  std::set<Agent*>::iterator it;
  for (it = agent_list_.begin(); it != agent_list_.end(); ++it) {
    if ((*it)->parent() == NULL) {
      to_del.push_back(*it);
    }
  }
  for (int i = 0; i < to_del.size(); ++i) {
    DelAgent(to_del[i]);
  }
}

void Context::DelAgent(Agent* m) {
  int n = agent_list_.erase(m);
  if (n == 1) {
    PyDelAgent(m->id());
    delete m;
    m = NULL;
  }
}

void Context::SchedBuild(Agent* parent, std::string proto_name, int t) {
  #pragma omp critical
  {
    if (t == -1) {
      t = time() + 1;
    }
    int pid = (parent != NULL) ? parent->id() : -1;
    ti_->SchedBuild(parent, proto_name, t);
    NewDatum("BuildSchedule")
        ->AddVal("ParentId", pid)
        ->AddVal("Prototype", proto_name)
        ->AddVal("SchedTime", time())
        ->AddVal("BuildTime", t)
        ->Record();
  }
}

void Context::SchedDecom(Agent* m, int t) {
  #pragma omp critical
  {
    if (t == -1) {
      t = time();
    }
    ti_->SchedDecom(m, t);
    NewDatum("DecomSchedule")
        ->AddVal("AgentId", m->id())
        ->AddVal("SchedTime", time())
        ->AddVal("DecomTime", t)
        ->Record();
  }
}

boost::uuids::uuid Context::sim_id() {
  return rec_->sim_id();
}

void Context::AddPrototype(std::string name, Agent* p) {
  AddPrototype(name, p, false);
}

void Context::AddPrototype(std::string name, Agent* p, bool overwrite) {
  if (!overwrite && protos_.find(name) != protos_.end()) {
    throw KeyError("Prototype name " + name + " has already been added" +
                   " and cannot be overwritten.");
  }

  protos_[name] = p;
  // explicit snapshot required for in situ (non-xml) prototype addition
  SimInit::SnapAgent(p);
  NewDatum("Prototypes")
      ->AddVal("Prototype", name)
      ->AddVal("AgentId", p->id())
      ->AddVal("Spec", p->spec())
      ->Record();

  std::string spec = p->spec();
  if (rec_ver_.count(spec) == 0) {
    rec_ver_.insert(spec);
    NewDatum("AgentVersions")
      ->AddVal("Spec", spec)
      ->AddVal("Version", p->version())
      ->Record();
  }
}

void Context::AddRecipe(std::string name, Composition::Ptr c) {
  recipes_[name] = c;
  NewDatum("Recipes")
      ->AddVal("Recipe", name)
      ->AddVal("QualId", c->id())
      ->Record();
}

Composition::Ptr Context::GetRecipe(std::string name) {
  if (recipes_.count(name) == 0) {
    throw KeyError("Invalid recipe name " + name);
  }
  return recipes_[name];
}

void Context::AddPackage(std::string name, double fill_min, double fill_max,
                         std::string strategy) {
  packages_[name] = Package::Create(name, fill_min, fill_max, strategy);
  NewDatum("Packages")
    ->AddVal("Package", name)
    ->AddVal("FillMin", fill_min)
    ->AddVal("FillMax", fill_max)
    ->AddVal("Strategy", strategy)
    ->Record();
}

Package::Ptr Context::GetPackageByName(std::string name) {
  if (packages_.count(name) == 0) {
    throw KeyError("Invalid package name " + name);
  }
  return packages_[name];
}

Package::Ptr Context::GetPackageById(int id) {
  if (id < 0) {
    throw ValueError("Invalid package id " + std::to_string(id));
  }
  // iterate through the list of packages to get the one package with the correct id
  std::map<std::string, Package::Ptr>::iterator it;
  for (it = packages_.begin(); it != packages_.end(); ++it) {
    if (it->second->id() == id) {
      return it->second;
    }
  }
  throw ValueError("Invalid package id " + std::to_string(id));
}

void Context::InitSim(SimInfo si) {
  NewDatum("Info")
      ->AddVal("Handle", si.handle)
      ->AddVal("InitialYear", si.y0)
      ->AddVal("InitialMonth", si.m0)
      ->AddVal("Duration", si.duration)
      ->AddVal("Seed", static_cast<int>(si.seed))
      ->AddVal("Stride", static_cast<int>(si.stride))
      ->AddVal("ParentSimId", si.parent_sim)
      ->AddVal("ParentType", si.parent_type)
      ->AddVal("BranchTime", si.branch_time)
      ->AddVal("CyclusVersion", std::string(version::core()))
      ->AddVal("CyclusVersionDescribe", std::string(version::describe()))
      ->AddVal("SqliteVersion", std::string(version::sqlite3()))
      ->AddVal("Hdf5Version", std::string(version::hdf5()))
      ->AddVal("BoostVersion", std::string(version::boost()))
      ->AddVal("LibXML2Version", std::string(version::xml2()))
      ->AddVal("CoinCBCVersion", std::string(version::coincbc()))
      ->Record();

  NewDatum("DecayMode")
      ->AddVal("Decay", si.decay)
      ->Record();

  NewDatum("InfoExplicitInv")
      ->AddVal("RecordInventory", si.explicit_inventory)
      ->AddVal("RecordInventoryCompact", si.explicit_inventory_compact)
      ->Record();

  // TODO: when the backends get uint64_t support, the static_cast here should
  // be removed.
  NewDatum("TimeStepDur")
      ->AddVal("DurationSecs", static_cast<int>(si.dt))
      ->Record();

  NewDatum("Epsilon")
      ->AddVal("GenericEpsilon", si.eps)
      ->AddVal("ResourceEpsilon", si.eps_rsrc)
      ->Record();

  NewDatum("XMLPPInfo")
      ->AddVal("LibXMLPlusPlusVersion", std::string(version::xmlpp()))
      ->Record();

  si_ = si;
  ti_->Initialize(this, si);
  rng_->Initialize(si);

}

int Context::time() {
  return ti_->time();
}

int Context::random() {
  return rng_->random();
}

double Context::random_01() {
  return rng_->random_01();
}

int Context::random_uniform_int(int low, int high) {
  return rng_->random_uniform_int(low, high);
}

double Context::random_uniform_real(double low, double high) {
  return rng_->random_uniform_real(low, high);
}

double Context::random_normal_real(double mean, double std_dev, double low,
                                     double high) {
  return rng_->random_normal_real(mean, std_dev, low, high);
}

int Context::random_normal_int(double mean, double std_dev, int low,
                                 int high) {
  return rng_->random_normal_int(mean, std_dev, low, high);
}

void Context::RegisterTimeListener(TimeListener* tl) {
  ti_->RegisterTimeListener(tl);
}

void Context::UnregisterTimeListener(TimeListener* tl) {
  ti_->UnregisterTimeListener(tl);
}

Datum* Context::NewDatum(std::string title) {
  return rec_->NewDatum(title);
}

void Context::Snapshot() {
  ti_->Snapshot();
}

void Context::KillSim() {
  ti_->KillSim();
}

}  // namespace cyclus
