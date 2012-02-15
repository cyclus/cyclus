// Timer.cpp
// Implements the Timer class

#include "Timer.h"

#include <string>
#include <iostream>

#include "InputXML.h"
#include "CycException.h"
#include "Logger.h"
#include "Material.h"

using namespace std;

Timer* Timer::instance_ = 0;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Timer::runSim() {
  time_ = -1;
  handlePreHistory();
  time_ = time0_;

  while (date_ < endDate()){
    if (date_.day() == 1){
      Material::decayMaterials(time_);
      sendTick();
      sendResolve();

      CLOG(LEV_INFO2) << "Current date: " << date_;
      CLOG(LEV_DEBUG3) << "The list of current tick listeners is: " << reportListeners();
    }
    
    int eom_day = lastDayOfMonth();
    for (int i = 1; i < eom_day+1; i++){
      sendDailyTasks();
      if (i == eom_day){
        sendTock();
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
    } catch (CycIndexException err) {
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
  for(vector<TimeAgent*>::iterator agent=tick_listeners_.begin();
      agent != tick_listeners_.end(); 
      agent++) {
    report += (*agent)->name() + " ";
  }
  return report;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Timer::handlePreHistory() {
  for(vector<TimeAgent*>::iterator agent=tick_listeners_.begin();
       agent != tick_listeners_.end(); 
       agent++) {
    try {
      (*agent)->handlePreHistory();
    } catch(CycException err) {
      LOG(LEV_ERROR, "none!") << "ERROR occured in handlePreHistory(): " << err.what();
    }
  }
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Timer::sendResolve() {
  for(vector<MarketModel*>::iterator agent=resolve_listeners_.begin();
       agent != resolve_listeners_.end(); 
       agent++) {
    try {
      CLOG(LEV_INFO4) << "Sending resolve to Model ID=" << (*agent)->ID()
                      << ", name=" << (*agent)->name();
      (*agent)->resolve();
    } catch(CycException err) {
      CLOG(LEV_ERROR) << "ERROR occured in sendResolve(): " << err.what();
    }
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Timer::sendTick() {
  for(vector<TimeAgent*>::iterator agent=tick_listeners_.begin();
       agent != tick_listeners_.end(); 
       agent++) {
    try {
      CLOG(LEV_INFO3) << "Sending tick to Model ID=" << (*agent)->ID()
                      << ", name=" << (*agent)->name();
      (*agent)->handleTick(time_);
    } catch(CycException err) {
      CLOG(LEV_ERROR) << "ERROR occured in sendTick(): " << err.what();
    }
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Timer::sendTock() {
  for(vector<TimeAgent*>::iterator agent=tick_listeners_.begin();
       agent != tick_listeners_.end(); 
       agent++) {
    try {
      CLOG(LEV_INFO3) << "Sending tock to Model ID=" << (*agent)->ID()
                      << ", name=" << (*agent)->name();
      (*agent)->handleTock(time_);
    } catch(CycException err) {
      CLOG(LEV_ERROR) << "ERROR occured in sendTock(): " << err.what();
    }
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Timer::sendDailyTasks() {
  for(vector<TimeAgent*>::iterator agent=tick_listeners_.begin();
       agent != tick_listeners_.end(); 
       agent++) {
    try {
      (*agent)->handleDailyTasks(time_,date_.day());
    } catch(CycException err) {
      CLOG(LEV_ERROR) << "ERROR occured in sendDailyTasks(): " << err.what();
    }
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
    instance_->initialize();
  }

  return instance_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Timer::initialize(int dur, int m0, int y0, int start, int decay) {

  if (m0 < 1 || m0 > 12)
    throw CycRangeException("Invalid month0; must be between 1 and 12 (inclusive).");

  if (y0 < 1942)
    throw CycRangeException("Invalid year0; the first man-made nuclear reactor was build in 1942");

  if (y0 > 2063)
    throw CycRangeException("Invalid year0; why start a simulation after we've got warp drive?: http://en.wikipedia.org/wiki/Warp_drive#Development_of_the_backstory");

  if (decay > dur)
    throw CycRangeException("Invalid decay interval; no decay occurs if the interval is greater than the simulation duriation. For no decay, use -1 .");
  Material::setDecay(decay);

  month0_ = m0;
  year0_ = y0;

  time0_ = start;
  time_ = start;
  simDur_ = dur;
  
  startDate_ = boost::gregorian::date(year0_,month0_,1);
  endDate_ = getEndDate(startDate_,simDur_);
  date_ = boost::gregorian::date(startDate_);

  CLOG(LEV_INFO1) << "Loading simulation to run from start="
                  << startDate_ << " to " << endDate_;
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
Timer::Timer() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int Timer::convertDate(int month, int year) {
  return (year - year0_) * 12 + (month - month0_) + time0_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pair<int, int> Timer::convertDate(int time) {
  int month = (time - time0_) % 12 + 1;
  int year = (time - time0_ - (month - 1)) / 12 + year0_;
  return make_pair(month, year);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Timer::load_simulation() { 
  
  int dur, m0, y0, sim0, dec;
  string dur_str, m0_str, y0_str, sim0_str, decay_str;

  xmlNodePtr cur = XMLinput->get_xpath_element("/simulation");
  // get duration
  dur_str = (XMLinput->get_xpath_content(cur,"duration"));
  // get start month
  m0_str = (XMLinput->get_xpath_content(cur,"startmonth"));
  // get start year
  y0_str = (XMLinput->get_xpath_content(cur,"startyear"));
  // get simulation start
  sim0_str = (XMLinput->get_xpath_content(cur,"simstart"));
  // get decay interval
  decay_str = (XMLinput->get_xpath_content(cur,"decay"));

  dur = strtol(dur_str.c_str(), NULL, 10);
  m0 = strtol(m0_str.c_str(), NULL, 10);
  y0 = strtol(y0_str.c_str(), NULL, 10);
  sim0 = strtol(sim0_str.c_str(), NULL, 10);
  dec = strtol(decay_str.c_str(), NULL, 10);

  TI->initialize(dur, m0, y0, sim0, dec);
}

