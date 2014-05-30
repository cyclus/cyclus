// timer.cc
// Implements the Timer class

#include "timer.h"
#include <string>
#include <iostream>

#include "error.h"
#include "logger.h"
#include "agent.h"
#include "sim_init.h"

namespace cyclus {

void Timer::RunSim() {
  CLOG(LEV_INFO1) << "Simulation set to run from start="
                  << 0 << " to end=" << si_.duration;
  CLOG(LEV_INFO1) << "Beginning simulation";

  ExchangeManager<Material> matl_manager(ctx_);
  ExchangeManager<Product> genrsrc_manager(ctx_);
  while (time_ < si_.duration) {
    CLOG(LEV_INFO2) << " Current time: " << time_;

    if (want_snapshot_) {
      want_snapshot_ = false;
      SimInit::Snapshot(ctx_);
      ctx_->NewDatum("Snapshots")
          ->AddVal("Time", time_)
          ->Record();
    }

    // run through phases
    DoBuild();
    DoTick();
    DoResEx(&matl_manager, &genrsrc_manager);
    DoTock();
    DoDecom();

    if (want_kill_) {
      break;
    }

    time_++;
  }


  ctx_->NewDatum("Finish")
      ->AddVal("EarlyTerm", want_kill_)
      ->AddVal("EndTime", time_)
      ->Record();

  time_++; // move time forward because snapshots are always "beginning of timestep"
  SimInit::Snapshot(ctx_); // always do a snapshot at the end of every simulation
  time_--;
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
  for (std::set<TimeListener*>::iterator agent = tickers_.begin();
       agent != tickers_.end();
       agent++) {
    (*agent)->Tick();
  }
}

void Timer::DoResEx(ExchangeManager<Material>* matmgr,
                    ExchangeManager<Product>* genmgr) {
  matmgr->Execute();
  genmgr->Execute();
}

void Timer::DoTock() {
  for (std::set<TimeListener*>::iterator agent = tickers_.begin();
       agent != tickers_.end();
       agent++) {
    (*agent)->Tock();
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
  tickers_.insert(agent);
}

void Timer::UnregisterTimeListener(TimeListener* tl) {
  tickers_.erase(tl);
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

} // namespace cyclus


