// Timer.cpp
// Implements the Timer class

#include "Timer.h"

#include <string>
#include <iostream>

#include "error.h"
#include "Logger.h"
#include "Material.h"
#include "EventManager.h"


namespace cyclus {

Timer* Timer::instance_ = 0;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Timer::runSim() {
  CLOG(LEV_INFO1) << "Simulation set to run from start="
                  << startDate_ << " to end=" << endDate_;
  time_ = time0_;
  CLOG(LEV_INFO1) << "Beginning simulation";
  while (date_ < endDate()){
    if (date_.day() == 1){
      CLOG(LEV_INFO2) << "Current date: " << date_ << " Current time: " << time_ << " {";
      CLOG(LEV_DEBUG3) << "The list of current tick listeners is: " << reportListeners();

      if (decay_interval_ > 0 && time_ > 0 && time_ % decay_interval_ == 0) {
        Material::decayMaterials();
      }

      sendTick();
      sendResolve();
    }
    
    int eom_day = lastDayOfMonth();
    for (int i = 1; i < eom_day+1; i++){
      sendDailyTasks();
      if (i == eom_day){
        sendTock();
        CLOG(LEV_INFO2) << "}";
      }
      date_ += boost::gregorian::days(1);
    }

    time_++;
  }

  // initiate deletion of models that don't have parents.
  // dealloc will propogate through hierarchy as models delete their children
  int count = 0;
  while (Model::getModelList().size() > 0) {
    Model* model = Model::getModelList().at(count);
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

int Timer::lastDayOfMonth(){
  int lastDay = 
    boost::gregorian::gregorian_calendar::end_of_month_day(date_.year(),
							   date_.month());
  return lastDay;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::string Timer::reportListeners() {
  std::string report = "";
  for(std::vector<TimeAgent*>::iterator agent=tick_listeners_.begin();
      agent != tick_listeners_.end(); 
      agent++) {
    report += (*agent)->name() + " ";
  }
  return report;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Timer::sendResolve() {
  for(std::vector<MarketModel*>::iterator agent=resolve_listeners_.begin();
       agent != resolve_listeners_.end(); 
       agent++) {
    CLOG(LEV_INFO3) << "Sending resolve to Model ID=" << (*agent)->ID()
                    << ", name=" << (*agent)->name() << " {";
    (*agent)->resolve();
    CLOG(LEV_INFO3) << "}";
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Timer::sendTick() {
  for(std::vector<TimeAgent*>::iterator agent=tick_listeners_.begin();
       agent != tick_listeners_.end(); 
       agent++) {
    CLOG(LEV_INFO3) << "Sending tick to Model ID=" << (*agent)->ID()
                    << ", name=" << (*agent)->name() << " {";
    (*agent)->handleTick(time_);
    CLOG(LEV_INFO3) << "}";
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Timer::sendTock() {
  for(std::vector<TimeAgent*>::iterator agent=tick_listeners_.begin();
       agent != tick_listeners_.end(); 
       agent++) {
    CLOG(LEV_INFO3) << "Sending tock to Model ID=" << (*agent)->ID()
                    << ", name=" << (*agent)->name() << " {";
    (*agent)->handleTock(time_);
    CLOG(LEV_INFO3) << "}";
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Timer::sendDailyTasks() {
  for(std::vector<TimeAgent*>::iterator agent=tick_listeners_.begin();
       agent != tick_listeners_.end(); 
       agent++) {
    (*agent)->handleDailyTasks(time_,date_.day());
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Timer::registerTickListener(TimeAgent* agent) {
  CLOG(LEV_INFO2) << "Model ID=" << agent->ID() << ", name=" << agent->name()
                  << " has registered to receive 'ticks' and 'tocks'.";
  tick_listeners_.push_back(agent);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Timer::registerResolveListener(MarketModel* agent) {
  CLOG(LEV_INFO2) << "Model ID=" << agent->ID() << ", name=" << agent->name()
                  << " has registered to receive 'resolves'.";
  resolve_listeners_.push_back(agent);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int Timer::time() {
  return time_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Timer* Timer::Instance() {
  // If we haven't created a Timer yet, create it, and then and return it
  // either way.
  if (0 == instance_) {
    instance_ = new Timer();
  }

  return instance_;
}

void Timer::reset() {
  resolve_listeners_.clear();
  tick_listeners_.clear();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Timer::initialize(int dur, int m0, int y0, int start, int decay) {
  reset();

  if (m0 < 1 || m0 > 12)
    throw ValueError("Invalid month0; must be between 1 and 12 (inclusive).");

  if (y0 < 1942)
    throw ValueError("Invalid year0; the first man-made nuclear reactor was build in 1942");

  if (y0 > 2063)
    throw ValueError("Invalid year0; why start a simulation after we've got warp drive?: http://en.wikipedia.org/wiki/Warp_drive#Development_of_the_backstory");

  if (decay > dur)
    throw ValueError("Invalid decay interval; no decay occurs if the interval is greater than the simulation duriation. For no decay, use -1 .");

  decay_interval_ = decay;

  month0_ = m0;
  year0_ = y0;

  time0_ = start;
  time_ = start;
  simDur_ = dur;
  
  startDate_ = boost::gregorian::date(year0_,month0_,1);
  endDate_ = getEndDate(startDate_,simDur_);
  date_ = boost::gregorian::date(startDate_);
  
  logTimeData();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
boost::gregorian::date Timer::getEndDate(boost::gregorian::date startDate, int simDur_){
  boost::gregorian::date endDate(startDate);
  endDate += boost::gregorian::months(simDur_-1);
  endDate += boost::gregorian::days(boost::gregorian::gregorian_calendar::end_of_month_day(endDate.year(),endDate.month())-1);
  return endDate;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int Timer::simDur() {
  return simDur_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Timer::Timer() :
  time_(0),
  time0_(0),
  simDur_(0),
  decay_interval_(0),
  month0_(0),
  year0_(0)
{
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int Timer::convertDate(int month, int year) {
  return (year - year0_) * 12 + (month - month0_) + time0_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::pair<int, int> Timer::convertDate(int time) {
  int month = (time - time0_) % 12 + 1;
  int year = (time - time0_ - (month - 1)) / 12 + year0_;
  return std::make_pair(month, year);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Timer::load_simulation(QueryEngine *qe) { 
  if (qe->nElementsMatchingQuery("simhandle") > 0) {
    handle_ = qe->getElementContent("simhandle");
  }

  // get duration
  std::string dur_str = qe->getElementContent("duration");
  int dur = strtol(dur_str.c_str(), NULL, 10);
  // get start month
  std::string m0_str = qe->getElementContent("startmonth");
  int m0 = strtol(m0_str.c_str(), NULL, 10);
  // get start year
  std::string y0_str = qe->getElementContent("startyear");
  int y0 = strtol(y0_str.c_str(), NULL, 10);
  // get simulation start
  std::string sim0_str = qe->getElementContent("simstart");
  int sim0 = strtol(sim0_str.c_str(), NULL, 10);
  // get decay interval
  std::string decay_str = qe->getElementContent("decay");
  int dec = strtol(decay_str.c_str(), NULL, 10);

  TI->initialize(dur, m0, y0, sim0, dec);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void Timer::logTimeData() {
  EM->newEvent("SimulationTimeInfo")
    ->addVal("SimHandle", handle_)
    ->addVal("InitialYear", year0_)
    ->addVal("InitialMonth", month0_)
    ->addVal("SimulationStart", time0_)
    ->addVal("Duration", simDur_)
    ->record();
}
} // namespace cyclus

