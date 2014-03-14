// timer.cc
// Implements the Timer class

#include "timer.h"
#include <string>
#include <iostream>

#include "error.h"
#include "logger.h"
#include "model.h"
#include "sim_init.h"

namespace cyclus {

void Timer::RunSim() {
  CLOG(LEV_INFO1) << "Simulation set to run from start="
                  << 0 << " to end=" << si_.duration;
  CLOG(LEV_INFO1) << "Beginning simulation";

  ExchangeManager<Material> matl_manager(ctx_);
  ExchangeManager<GenericResource> genrsrc_manager(ctx_);
  while (time_ < si_.duration) {
    CLOG(LEV_INFO2) << " Current time: " << time_;
    if (si_.decay_period > 0 && time_ > 0 && time_ % si_.decay_period == 0) {
      Material::DecayAll(time_);
    }

    // run through phases
    DoBuild();
    DoTick();
    DoResEx(&matl_manager, &genrsrc_manager);
    DoTock();
    DoDecom();

    if (want_snapshot_) {
      want_snapshot_ = false;
      SimInit::Snapshot(ctx_);
    }

    time_++;
  }
}

void Timer::DoBuild() {
  // build queued agents
  std::vector<std::pair<std::string, Model*> > build_list = build_queue_[time_];
  for (int i = 0; i < build_list.size(); ++i) {
    Model* m = ctx_->CreateModel<Model>(build_list[i].first);
    Model* parent = build_list[i].second;
    m->Build(parent);
    parent->BuildNotify(m);
  }
}

void Timer::DoTick() {
  for (std::set<TimeListener*>::iterator agent = tickers_.begin();
       agent != tickers_.end();
       agent++) {
    (*agent)->Tick(time_);
  }
}

void Timer::DoResEx(ExchangeManager<Material>* matmgr,
                    ExchangeManager<GenericResource>* genmgr) {
  matmgr->Execute();
  genmgr->Execute();
}

void Timer::DoTock() {
  for (std::set<TimeListener*>::iterator agent = tickers_.begin();
       agent != tickers_.end();
       agent++) {
    (*agent)->Tock(time_);
  }
}

void Timer::DoDecom() {
  // decommission queued agents
  std::vector<Model*> decom_list = decom_queue_[time_];
  for (int i = 0; i < decom_list.size(); ++i) {
    Model* m = decom_list[i];
    m->parent()->DecomNotify(m);
    m->Decommission();
  }
}

void Timer::RegisterTimeListener(TimeListener* agent) {
  tickers_.insert(agent);
}

void Timer::UnregisterTimeListener(TimeListener* tl) {
  tickers_.erase(tl);
}

void Timer::SchedBuild(Model* parent, std::string proto_name, int t) {
  if (t <= time_) {
    throw ValueError("Cannot schedule build for t < [current-time]");
  }
  build_queue_[t].push_back(std::make_pair(proto_name, parent));
}

void Timer::SchedDecom(Model* m, int t) {
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
  } else if (si.y0 < 1942) {
    throw ValueError("Invalid year0; the first man-made nuclear reactor was build in 1942");
  } else if (si.y0 > 2063) {
    throw ValueError("Invalid year0; why start a simulation after we've got warp drive?: http://en.wikipedia.org/wiki/Warp_drive#Development_of_the_backstory");
  } else if (si.decay_period > si.duration) {
    throw ValueError("Invalid decay interval; no decay occurs if the interval is greater than the simulation duriation. For no decay, use -1 .");
  }

  ctx_ = ctx;
  time_ = 0;
  si_ = si;
}

int Timer::dur() {
  return si_.duration;
}

Timer::Timer() : time_(0), si_(0), want_snapshot_(false) {}

} // namespace cyclus


