// Timer.cpp
// Implements the Timer class

#include "InputXML.h"
#include "Timer.h"
#include "GenException.h"
#include <iostream>

Timer* Timer::instance_ = 0;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Timer::runSim() {
  time_ = -1;
  handlePreHistory();
  time_ = time0_;

  for (int i = time_; i < simDur_; i++) {
    
    // Give a status report, periodically.
    // (monthly during testing, change to (i % 12 == 0) for annual reporting.
    if (i % 1 == 0) {
      cout << "Current time: " << i << endl;
    }
    
    // Tell the Logician to handle this month.
    LI->decayMaterials(time_);
    sendTick();
    LI->resolveMarkets();
    sendTock();
    
    // Increment the time.
    time_++;
  }
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Timer::handlePreHistory() {
  for(vector<TimeAgent*>::iterator agent=time_agents_.begin();
       agent != time_agents_.end(); 
       agent++) {
    (*agent)->handlePreHistory();
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Timer::sendTick() {
  for(vector<TimeAgent*>::iterator agent=time_agents_.begin();
       agent != time_agents_.end(); 
       agent++) {
    (*agent)->handleTick(time_);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Timer::sendTock() {
  for(vector<TimeAgent*>::iterator agent=time_agents_.begin();
       agent != time_agents_.end(); 
       agent++) {
    (*agent)->handleTock(time_);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Timer::registerAgent(TimeAgent* agent) {
  time_agents_.push_back(agent);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int Timer::getTime() {
	return time_;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Timer* Timer::Instance() 
{
	// If we haven't created a Timer yet, create it, and then and return it
	// either way.
	if (0 == instance_) {
		instance_ = new Timer();
	}

	return instance_;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Timer::initialize(int dur, int m0, int y0, int start, int decay) {

	if (m0 < 1 || m0 > 12)
		throw GenException("Invalid month0; must be between 1 and 12 (inclusive).");

	if (y0 < 1942)
		throw GenException("Invalid year0; the first man-made nuclear reactor was build in 1942");

	if (y0 > 2063)
		throw GenException("Invalid year0; why start a simulation after we've got warp drive?: http://en.wikipedia.org/wiki/Warp_drive#Development_of_the_backstory");

	if (decay > dur)
		throw GenException("Invalid decay interval; no decay occurs if the interval is greater than the simulation duriation. For no decay, use -1 .");
  LI->setDecay(decay);

	month0_ = m0;
	year0_ = y0;

	time0_ = start;
	time_ = start;
	simDur_ = dur;
  
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int Timer::getSimDur() {
	return simDur_;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Timer::Timer() 
{

}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int Timer::convertDate(int month, int year)
{
	return (year - year0_) * 12 + (month - month0_) + time0_;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pair<int, int> Timer::convertDate(int time)
{
	int month = (time - time0_) % 12 + 1;
	int year = (time - time0_ - (month - 1)) / 12 + year0_;
	return make_pair(month, year);
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Timer::load_simulation()
{ 
  
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

  dur = atoi(dur_str.c_str());
  m0 = atoi(m0_str.c_str());
  y0 = atoi(y0_str.c_str());
  sim0 = atoi(sim0_str.c_str());
  dec = atoi(decay_str.c_str());

  TI->initialize(dur, m0, y0, sim0, dec);
}

