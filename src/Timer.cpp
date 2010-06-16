// Timer.cpp
// Implements the Timer class

#include "Timer.h"
#include "GenException.h"
#include <iostream>

Timer* Timer::_instance = 0;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Timer::runSim()
{
	for (int i = time; i < simDur; i++) {
		
		// Give a status report, periodically.
		if (i % 10 == 0)
		 	cout << "Current time: " << i << endl;

		// Tell the manager to handle this month.
//		myManager->handleTick(time);
//		myManager->handleTock(time);
			
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
//	myManager = MI;
//	myManager->setSimDur(simDur);
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
