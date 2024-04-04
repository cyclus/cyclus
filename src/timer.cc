#include "platform.h"
// Implements the Timer class
#include "timer.h"

#include <iostream>
#include <string>
#if CYCLUS_IS_PARALLEL
#include <omp.h>
#endif // CYCLUS_IS_PARALLEL

#include "agent.h"
#include "error.h"
#include "logger.h"
#include "pyhooks.h"
#include "sim_init.h"

namespace cyclus {

void Timer::RunSim() {
  CLOG(LEV_INFO1) << "Simulation set to run from start="
                  << 0 << " to end=" << si_.duration;
  CLOG(LEV_INFO1) << "Beginning simulation";

  ExchangeManager<Material> matl_manager(ctx_);
  ExchangeManager<Product> genrsrc_manager(ctx_);
  while (time_ < si_.duration) {
    CLOG(LEV_INFO1) << "Current time: " << time_;

    if (want_snapshot_) {
      want_snapshot_ = false;
      SimInit::Snapshot(ctx_);
    }

    // run through phases
    DoBuild();
    CLOG(LEV_INFO2) << "Beginning Tick for time: " << time_;
    DoTick();
    CLOG(LEV_INFO2) << "Beginning DRE for time: " << time_;
    DoResEx(&matl_manager, &genrsrc_manager);
    CLOG(LEV_INFO2) << "Beginning Tock for time: " << time_;
    DoTock();
    CLOG(LEV_INFO2) << "Beginning Decision for time: " << time_;
    DoDecision();
    DoDecom();

#ifdef CYCLUS_WITH_PYTHON
    EventLoop();
#endif

    time_++;

    if (want_kill_) {
      break;
    }
  }

  ctx_->NewDatum("Finish")
      ->AddVal("EarlyTerm", want_kill_)
      ->AddVal("EndTime", time_-1)
      ->Record();

  SimInit::Snapshot(ctx_);  // always do a snapshot at the end of every simulation
}

void Timer::DoBuild() {
  // build queued agents
  std::vector<std::pair<std::string, Agent*> > build_list = build_queue_[time_];
  for (int i = 0; i < build_list.size(); ++i) {
    Agent* m = ctx_->CreateAgent<Agent>(build_list[i].first);
    Agent* parent = build_list[i].second;
    CLOG(LEV_INFO3) << "Building a " << build_list[i].first
                    << " from parent " << build_list[i].second;
    m->Build(parent);
    if (parent != NULL) {
      parent->BuildNotify(m);
    } else {
      CLOG(LEV_DEBUG1) << "Hey! Listen! Built an Agent without a Parent.";
    }
  }
}

void Timer::DoTick() {
  // partition our tickers_ map into C++ agents and python agents.
  // Python agents segfault when Tick'ed in parallel so we need to 
  // run them serially
  std::vector<TimeListener*> cpp_agents;
  std::vector<TimeListener*> py_agents;
  for (std::pair<int, TimeListener*> pair : tickers_) {
    if (pair.second->IsShim()) {
      py_agents.push_back(pair.second);
    }
    else {
      cpp_agents.push_back(pair.second);
    }
  }
  
  for (TimeListener* agent : py_agents) {
    agent->Tick();
  }

  #pragma omp parallel for
  for (TimeListener* agent : cpp_agents) {
    agent->Tick();
  }
}

void Timer::DoResEx(ExchangeManager<Material>* matmgr,
                    ExchangeManager<Product>* genmgr) {
  matmgr->Execute();
  genmgr->Execute();
}

void Timer::DoTock() {
  // partition our tickers_ map into C++ agents and python agents.
  // Python agents segfault when Tock'ed in parallel so we need to 
  // run them serially
  std::vector<TimeListener*> cpp_agents;
  std::vector<TimeListener*> py_agents;
  for (std::pair<int, TimeListener*> pair : tickers_) {
    if (pair.second->IsShim()) {
      py_agents.push_back(pair.second);
    }
    else {
      cpp_agents.push_back(pair.second);
    }
  }
  
  for (TimeListener* agent : py_agents) {
    agent->Tock();
  }

  #pragma omp parallel for
  for (TimeListener* agent : cpp_agents) {
    agent->Tock();
  }

  if (si_.explicit_inventory || si_.explicit_inventory_compact) {
    std::set<Agent*> ags = ctx_->agent_list_;
    #pragma omp parallel for
    for (int i = 0; i < ags.size(); i++) {
      std::set<Agent*>::iterator it = ags.begin();
      std::advance(it, i);
      Agent* a = *it;
      if (a->enter_time() == -1) {
        continue; // skip agents that aren't alive
      }
      RecordInventories(a);
    }
  }
}

void Timer::DoDecision() {
  for (std::map<int, TimeListener*>::iterator agent = tickers_.begin();
       agent != tickers_.end();
       agent++) {
    agent->second->Decision();
  }
}

void Timer::RecordInventories(Agent* a) {
  Inventories invs = a->SnapshotInv();
  Inventories::iterator it2;
  for (it2 = invs.begin(); it2 != invs.end(); ++it2) {
    std::string name = it2->first;
    std::vector<Resource::Ptr> mats = it2->second;
    if (mats.empty() || ResCast<Material>(mats[0]) == NULL) {
      continue; // skip non-material inventories
    }

    Material::Ptr m = ResCast<Material>(mats[0]->Clone());
    for (int i = 1; i < mats.size(); i++) {
      m->Absorb(ResCast<Material>(mats[i]->Clone()));
    }
    RecordInventory(a, name, m);
  }
}

void Timer::RecordInventory(Agent* a, std::string name, Material::Ptr m) {
  if (si_.explicit_inventory) {
    CompMap c = m->comp()->mass();
    compmath::Normalize(&c, m->quantity());
    CompMap::iterator it;
    for (it = c.begin(); it != c.end(); ++it) {
      ctx_->NewDatum("ExplicitInventory")
          ->AddVal("AgentId", a->id())
          ->AddVal("Time", time_)
          ->AddVal("InventoryName", name)
          ->AddVal("NucId", it->first)
          ->AddVal("Quantity", it->second)
          ->AddVal("Units", m->units())
          ->Record();
    }
  }

  if (si_.explicit_inventory_compact) {
    CompMap c = m->comp()->mass();
    compmath::Normalize(&c, 1);
    ctx_->NewDatum("ExplicitInventoryCompact")
        ->AddVal("AgentId", a->id())
        ->AddVal("Time", time_)
        ->AddVal("InventoryName", name)
        ->AddVal("Quantity", m->quantity())
        ->AddVal("Units", m->units())
        ->AddVal("Composition", c)
        ->Record();
  }
}

void Timer::DoDecom() {
  // decommission queued agents
  std::vector<Agent*> decom_list = decom_queue_[time_];
  for (int i = 0; i < decom_list.size(); ++i) {
    Agent* m = decom_list[i];
    if (m->parent() != NULL) {
      m->parent()->DecomNotify(m);
    }
    m->Decommission();
  }
}

void Timer::RegisterTimeListener(TimeListener* agent) {
  tickers_[agent->id()] = agent;
}

void Timer::UnregisterTimeListener(TimeListener* tl) {
  tickers_.erase(tl->id());
}

void Timer::SchedBuild(Agent* parent, std::string proto_name, int t) {
  if (t <= time_) {
    throw ValueError("Cannot schedule build for t < [current-time]");
  }
  build_queue_[t].push_back(std::make_pair(proto_name, parent));
}

void Timer::SchedDecom(Agent* m, int t) {
  if (t < time_) {
    throw ValueError("Cannot schedule decommission for t < [current-time]");
  }

  // It is possible that a single agent may be scheduled for decommissioning
  // multiple times. If this happens, we cannot just add it to the queue again
  // - the duplicate entries will result in a double delete attempt and
  // segfaults and otherwise bad things.  Remove previous decommissionings
  // before scheduling this new one.
  std::map<int, std::vector<Agent*> >::iterator it;
  bool done = false;
  for (it = decom_queue_.begin(); it != decom_queue_.end(); ++it) {
    int t = it->first;
    std::vector<Agent*> ags = it->second;
    for (int i = 0; i < ags.size(); i++) {
      if (ags[i] == m) {
        CLOG(LEV_WARN) << "scheduled over previous decommissioning of " << m->id();
        decom_queue_[t].erase(decom_queue_[t].begin()+i);
        done = true;
        break;
      }
    }
    if (done) {
      break;
    }
  }

  decom_queue_[t].push_back(m);
}

int Timer::time() {
  return time_;
}

void Timer::Reset() {
  tickers_.clear();
  build_queue_.clear();
  decom_queue_.clear();
  si_ = SimInfo(0);
}

void Timer::Initialize(Context* ctx, SimInfo si) {
  if (si.m0 < 1 || si.m0 > 12) {
    throw ValueError("Invalid month0; must be between 1 and 12 (inclusive).");
  }

  want_kill_ = false;
  ctx_ = ctx;
  time_ = 0;
  si_ = si;

  if (si.branch_time > -1) {
    time_ = si.branch_time;
  }
}

int Timer::dur() {
  return si_.duration;
}

Timer::Timer() : time_(0), si_(0), want_snapshot_(false), want_kill_(false) {}

}  // namespace cyclus
