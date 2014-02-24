// timer.cc
// Implements the Timer class

#include "timer.h"
#include <string>
#include <iostream>

#include "error.h"
#include "exchange_manager.h"
#include "generic_resource.h"
#include "logger.h"
#include "material.h"
#include "model.h"

namespace cyclus {

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Timer::RunSim(Context* ctx) {
  CLOG(LEV_INFO1) << "Simulation set to run from start="
                  << 0 << " to end=" << dur_;
  CLOG(LEV_INFO1) << "Beginning simulation";

  ExchangeManager<Material> matl_manager(ctx);
  ExchangeManager<GenericResource> genrsrc_manager(ctx);
  while (time_ < dur_) {
    CLOG(LEV_INFO2) << " Current time: " << time_;
    if (decay_interval_ > 0 && time_ > 0 && time_ % decay_interval_ == 0) {
      Material::DecayAll(time_);
    }

    // build queued agents
    std::vector<std::pair<std::string, Model*> > build_list = build_queue_[time_];
    for (int i = 0; i < build_list.size(); ++i) {
      Model* m = ctx->CreateModel<Model>(build_list[i].first);
      Model* parent = build_list[i].second;
      m->Build(parent);
      parent->BuildNotify(m);
    }

    // run through phases
    SendTick();
    matl_manager.Execute();
    genrsrc_manager.Execute();
    SendTock();

    // decommission queued agents
    std::vector<Model*> decom_list = decom_queue_[time_];
    for (int i = 0; i < decom_list.size(); ++i) {
      Model* m = decom_list[i];
      m->parent()->DecomNotify(m);
      m->Decommission();
    }

    time_++;
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Timer::SendTick() {
  for (std::set<TimeListener*>::iterator agent = tickers_.begin();
       agent != tickers_.end();
       agent++) {
    (*agent)->Tick(time_);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Timer::SendTock() {
  for (std::set<TimeListener*>::iterator agent = tickers_.begin();
       agent != tickers_.end();
       agent++) {
    (*agent)->Tock(time_);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Timer::RegisterTimeListener(TimeListener* agent) {
  tickers_.insert(agent);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Timer::UnregisterTimeListener(TimeListener* tl) {
  tickers_.erase(tl);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Timer::SchedBuild(Model* parent, std::string proto_name, int t) {
  if (t <= time_) {
    throw ValueError("Cannot schedule build for t < [current-time]");
  }
  build_queue_[t].push_back(std::make_pair(proto_name, parent));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Timer::SchedDecom(Model* m, int t) {
  if (t <= time_) {
    throw ValueError("Cannot schedule decommission for t < [current-time]");
  }
  decom_queue_[t].push_back(m);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int Timer::time() {
  return time_;
}

void Timer::Reset() {
  tickers_.clear();
  build_queue_.clear();
  decom_queue_.clear();

  decay_interval_ = 0;
  month0_ = 0;
  year0_ = 0;
  time_ = 0;
  dur_ = 0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Timer::Initialize(Context* ctx, int dur, int m0, int y0,
                       int decay, std::string handle) {
  if (m0 < 1 || m0 > 12) {
    throw ValueError("Invalid month0; must be between 1 and 12 (inclusive).");
  }

  if (y0 < 1942) {
    throw ValueError("Invalid year0; the first man-made nuclear reactor was build in 1942");
  }

  if (y0 > 2063) {
    throw ValueError("Invalid year0; why start a simulation after we've got warp drive?: http://en.wikipedia.org/wiki/Warp_drive#Development_of_the_backstory");
  }

  if (decay > dur) {
    throw ValueError("Invalid decay interval; no decay occurs if the interval is greater than the simulation duriation. For no decay, use -1 .");
  }

  decay_interval_ = decay;

  month0_ = m0;
  year0_ = y0;

  time_ = 0;
  dur_ = dur;

  LogTimeData(ctx, handle);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int Timer::dur() {
  return dur_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Timer::Timer() :
  time_(0),
  dur_(0),
  decay_interval_(0),
  month0_(0),
  year0_(0) {}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Timer::LogTimeData(Context* ctx, std::string handle) {
  ctx->NewDatum("Info")
  ->AddVal("Handle", handle)
  ->AddVal("InitialYear", year0_)
  ->AddVal("InitialMonth", month0_)
  ->AddVal("Start", time_)
  ->AddVal("Duration", dur_)
  ->AddVal("DecayInterval", decay_interval_)
  ->Record();
}
} // namespace cyclus


