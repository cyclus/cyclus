// timer.cc
// Implements the Timer class

#include "timer.h"

#include <string>
#include <iostream>

#include "error.h"
#include "logger.h"
#include "material.h"

namespace cyclus {

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Timer::RunSim() {
  CLOG(LEV_INFO1) << "Simulation set to run from start="
                  << start_date_ << " to end=" << end_date_;
  time_ = start_time_;
  CLOG(LEV_INFO1) << "Beginning simulation";
  while (date_ < endDate()) {
    if (date_.day() == 1) {
      CLOG(LEV_INFO2) << "Current date: " << date_ << " Current time: " << time_ <<
                      " {";
      CLOG(LEV_DEBUG3) << "The list of current tick listeners is: " <<
                       ReportListeners();

      if (decay_interval_ > 0 && time_ > 0 && time_ % decay_interval_ == 0) {
        Material::DecayAll(time_);
      }

      SendTick();
      SendResolve();
    }

    int eom_day = LastDayOfMonth();
    for (int i = 1; i < eom_day + 1; i++) {
      SendDailyTasks();
      if (i == eom_day) {
        SendTock();
        CLOG(LEV_INFO2) << "}";
      }
      date_ += boost::gregorian::days(1);
    }

    time_++;
  }

  // initiate deletion of models that don't have parents.
  // dealloc will propogate through hierarchy as models delete their children
  int count = 0;
  while (Model::GetModelList().size() > 0) {
    Model* model = Model::GetModelList().at(count);
    try {
      model->parent();
    } catch (ValueError err) {
      delete model;
      count = 0;
      continue;
    }
    count++;
  }
}

int Timer::LastDayOfMonth() {
  int lastDay =
    boost::gregorian::gregorian_calendar::end_of_month_day(date_.year(),
                                                           date_.month());
  return lastDay;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::string Timer::ReportListeners() {
  std::string report = "";
  for (std::vector<TimeAgent*>::iterator agent = tick_listeners_.begin();
       agent != tick_listeners_.end();
       agent++) {
    report += (*agent)->name() + " ";
  }
  return report;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Timer::SendResolve() {
  for (std::vector<MarketModel*>::iterator agent = resolve_listeners_.begin();
       agent != resolve_listeners_.end();
       agent++) {
    CLOG(LEV_INFO3) << "Sending resolve to Model ID=" << (*agent)->ID()
                    << ", name=" << (*agent)->name() << " {";
    (*agent)->Resolve();
    CLOG(LEV_INFO3) << "}";
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Timer::SendTick() {
  for (std::vector<TimeAgent*>::iterator agent = tick_listeners_.begin();
       agent != tick_listeners_.end();
       agent++) {
    CLOG(LEV_INFO3) << "Sending tick to Model ID=" << (*agent)->ID()
                    << ", name=" << (*agent)->name() << " {";
    (*agent)->HandleTick(time_);
    CLOG(LEV_INFO3) << "}";
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Timer::SendTock() {
  for (std::vector<TimeAgent*>::iterator agent = tick_listeners_.begin();
       agent != tick_listeners_.end();
       agent++) {
    CLOG(LEV_INFO3) << "Sending tock to Model ID=" << (*agent)->ID()
                    << ", name=" << (*agent)->name() << " {";
    (*agent)->HandleTock(time_);
    CLOG(LEV_INFO3) << "}";
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Timer::SendDailyTasks() {
  for (std::vector<TimeAgent*>::iterator agent = tick_listeners_.begin();
       agent != tick_listeners_.end();
       agent++) {
    (*agent)->HandleDailyTasks(time_, date_.day());
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Timer::RegisterTickListener(TimeAgent* agent) {
  CLOG(LEV_INFO2) << "Model ID=" << agent->ID() << ", name=" << agent->name()
                  << " has registered to receive 'ticks' and 'tocks'.";
  tick_listeners_.push_back(agent);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Timer::RegisterResolveListener(MarketModel* agent) {
  CLOG(LEV_INFO2) << "Model ID=" << agent->ID() << ", name=" << agent->name()
                  << " has registered to receive 'resolves'.";
  resolve_listeners_.push_back(agent);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int Timer::time() {
  return time_;
}

void Timer::reset() {
  resolve_listeners_.clear();
  tick_listeners_.clear();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Timer::Initialize(Context* ctx, int dur, int m0, int y0, int start,
                       int decay, std::string handle) {
  reset();

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

  start_date_ = boost::gregorian::date(year0_, month0_, 1);
  end_date_ = GetEndDate(start_date_, dur_);
  date_ = boost::gregorian::date(start_date_);

  LogTimeData(ctx, handle);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
boost::gregorian::date Timer::GetEndDate(boost::gregorian::date startDate,
                                         int dur_) {
  boost::gregorian::date endDate(startDate);
  endDate += boost::gregorian::months(dur_ - 1);
  endDate += boost::gregorian::days(
               boost::gregorian::gregorian_calendar::end_of_month_day(endDate.year(),
                   endDate.month()) - 1);
  return endDate;
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
  year0_(0) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int Timer::ConvertDate(int month, int year) {
  return (year - year0_) * 12 + (month - month0_) + start_time_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::pair<int, int> Timer::ConvertDate(int time) {
  int month = (time - start_time_) % 12 + 1;
  int year = (time - start_time_ - (month - 1)) / 12 + year0_;
  return std::make_pair(month, year);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Timer::LogTimeData(Context* ctx, std::string handle) {
  ctx->NewEvent("SimulationTimeInfo")
  ->AddVal("SimHandle", handle)
  ->AddVal("InitialYear", year0_)
  ->AddVal("InitialMonth", month0_)
  ->AddVal("SimulationStart", start_time_)
  ->AddVal("Duration", dur_)
  ->Record();
}
} // namespace cyclus

