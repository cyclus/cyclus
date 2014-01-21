// timer.cc
// Implements the Timer class

#include "timer.h"
#include <string>
#include <iostream>

#include "error.h"
#include "logger.h"
#include "material.h"
#include "generic_resource.h"
#include "exchange_manager.h"

namespace cyclus {

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Timer::RunSim(Context* ctx) {
  CLOG(LEV_INFO1) << "Simulation set to run from start="
                  << start_time_ << " to end=" << start_time_ + dur_;
  CLOG(LEV_INFO1) << "Beginning simulation";

  time_ = start_time_;
  ExchangeManager<Material> matl_manager(ctx);
  ExchangeManager<GenericResource> genrsrc_manager(ctx);
  while (time_ < start_time_ + dur_) {
    CLOG(LEV_INFO2) << " Current time: " << time_;
    if (decay_interval_ > 0 && time_ > 0 && time_ % decay_interval_ == 0) {
      Material::DecayAll(time_);
    }
      
    // provides robustness when listeners are added during ticks/tocks
    for (int i = 0; i < new_tickers_.size(); ++i) {
      tick_listeners_.push_back(new_tickers_[i]);
    }
    new_tickers_.clear();

    SendTick();
    matl_manager.Execute();
    genrsrc_manager.Execute();
    SendTock();
    
    time_++;
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Timer::SendTick() {
  for (std::vector<TimeListener*>::iterator agent = tick_listeners_.begin();
       agent != tick_listeners_.end();
       agent++) {
    (*agent)->Tick(time_);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Timer::SendTock() {
  for (std::vector<TimeListener*>::iterator agent = tick_listeners_.begin();
       agent != tick_listeners_.end();
       agent++) {
    (*agent)->Tock(time_);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Timer::RegisterTickListener(TimeListener* agent) {
  new_tickers_.push_back(agent);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int Timer::time() {
  return time_;
}

void Timer::Reset() {
  tick_listeners_.clear();

  decay_interval_ = 0;
  month0_ = 0;
  year0_ = 0;
  start_time_ = 0;
  time_ = 0;
  dur_ = 0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Timer::Initialize(Context* ctx, int dur, int m0, int y0, int start,
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

  start_time_ = start;
  time_ = start;
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
  start_time_(0),
  dur_(0),
  decay_interval_(0),
  month0_(0),
  year0_(0) {}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Timer::LogTimeData(Context* ctx, std::string handle) {
  ctx->NewDatum("SimulationTimeInfo")
  ->AddVal("SimHandle", handle)
  ->AddVal("InitialYear", year0_)
  ->AddVal("InitialMonth", month0_)
  ->AddVal("SimulationStart", start_time_)
  ->AddVal("Duration", dur_)
  ->AddVal("DecayInterval", decay_interval_)
  ->Record();
}
} // namespace cyclus

