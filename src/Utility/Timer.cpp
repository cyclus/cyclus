// Timer.cpp
// Implements the Timer class

#include "InputXML.h"
#include "Timer.h"
#include "GenException.h"
#include <iostream>

Timer* Timer::_instance = 0;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Timer::runSim()
{

  time = -1;
  LI->handlePreHistory();
  time = time0;

  for (int i = time; i < simDur; i++) {
    
    // Give a status report, periodically.
    // (monthly during testing, change to (i % 12 == 0) for annual reporting.
    if (i % 1 == 0)
      cout << "Current time: " << i << endl;
    
    // Tell the Logician to handle this month.
    LI-> handleTimeStep(time);
    
    // Increment the time.
    time ++;
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int Timer::getTime() {
	return time;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Timer* Timer::Instance() 
{
	// If we haven't created a Timer yet, create it, and then and return it
	// either way.
	if (0 == _instance) {
		_instance = new Timer();
	}

	return _instance;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Timer::initialize(int dur, int m0, int y0, int start) {

	if (m0 < 1 || m0 > 12)
		throw GenException("Invalid month0; must be between 1 and 12 (inclusive).");

	if (y0 < 1942)
		throw GenException("Invalid year0; the first man-made nuclear reactor was build in 1942");

	if (y0 > 2063)
		throw GenException("Invalid year0; why start a simulation after we've got warp drive?: http://en.wikipedia.org/wiki/Warp_drive#Development_of_the_backstory");

	month0 = m0;
	year0 = y0;

	time0 = start;
	time = start;
	simDur = dur;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int Timer::getSimDur() {
	return simDur;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Timer::Timer() 
{

}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int Timer::convertDate(int month, int year)
{
	return (year - year0) * 12 + (month - month0) + time0;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pair<int, int> Timer::convertDate(int time)
{
	int month = (time - time0) % 12 + 1;
	int year = (time - time0 - (month - 1)) / 12 + year0;
	return make_pair(month, year);
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Timer::load_simulation()
{ 
  
  int dur, m0, y0, sim0;
  string dur_str, m0_str, y0_str, sim0_str;

  xmlNodePtr cur = XMLinput->get_xpath_element("/simulation");
  // get duration
  dur_str = (XMLinput->get_xpath_content(cur,"duration"));
  // get start month
  m0_str = (XMLinput->get_xpath_content(cur,"startmonth"));
  // get start year
  y0_str = (XMLinput->get_xpath_content(cur,"startyear"));
  // get simulation start
  sim0_str = (XMLinput->get_xpath_content(cur,"simstart"));

  dur = atoi(dur_str.c_str());
  m0 = atoi(m0_str.c_str());
  y0 = atoi(y0_str.c_str());
  sim0 = atoi(sim0_str.c_str());

  TI->initialize(dur, m0, y0, sim0);
}

