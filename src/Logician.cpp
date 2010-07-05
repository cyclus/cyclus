// Logician.cpp
// Implements the Logician class.
#include <math.h>
#include "Logician.h"
#include "Market.h"
#include "Facility.h"
#include "Commodity.h"
#include "Timer.h"
#include "GenException.h"


Logician* Logician::_instance = 0;
//MarketMap Logician::mktMap = MarketMap();

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Logician* Logician::Instance() {
	// If we haven't created a Logician yet, create and return it.
	if (0 == _instance)
		_instance = new Logician();
	
	return _instance;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Logician::Logician() {
	facilities = vector<Facility*>();
	markets = vector<Market*>();
	mktMap = MarketMap();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Logician::handleTick(int time) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Logician::handleTock(int time) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int Logician::getNumFacs() const
{
	return facilities.size();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int Logician::getNumMarkets() const
{
	return markets.size();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Logician::setSimDur(int numMonths) {
	simDur = numMonths;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Facility* Logician::getFacility(int ID)
{
	// Iterate through the vector of Facilities and return a pointer to the 
	// one with the given ID, if it exists.
	vector<Facility*>::iterator iter;
	for (iter = facilities.begin(); iter != facilities.end(); iter ++)
		if ((*iter)->getSN() == ID)
			return *iter;

	throw GenException("Error: tried to get Facility that didn't exist.");
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//
Facility* Logician::getFacility(string name)
{
	// Iterate through the vector of Facilities and return a pointer to the 
	// one with the given ID, if it exists.
	vector<Facility*>::iterator iter;
	for (iter = facilities.begin(); iter != facilities.end(); iter ++)
		if ((*iter)->getName() == name)
			return *iter;

	throw GenException("Error: tried to get Facility that didn't exist.");
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pair<vector<Facility*>::iterator, vector<Facility*>::iterator> Logician::getFacilities()
{
	return make_pair(facilities.begin(), facilities.end());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Market* Logician::getMarket(int ID)
{
	// Iterate through the vector of Markets and return a pointer to the 
	// one with the given ID, if it exists.
	vector<Market*>::iterator iter;
	for (iter = markets.begin(); iter != markets.end(); iter ++)
		if ((*iter)->getSN() == ID)
			return *iter;

	throw GenException("Error: tried to get Market that didn't exist.");
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Market* Logician::getMarket(string name)
{
	// Iterate through the vector of Markets and return a pointer to the 
	// one with the given ID, if it exists.
	vector<Market*>::iterator iter;
	for (iter = markets.begin(); iter != markets.end(); iter ++)
		if ((*iter)->getName() == name)
			return *iter;

	throw GenException("Error: tried to get Market that didn't exist.");
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pair<vector<Market*>::iterator, vector<Market*>::iterator> Logician::getMarkets()
{
	return make_pair(markets.begin(), markets.end());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Logician::~Logician()
{
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Logician::handleEnd(int time)
{
	// Perform any final logician tasks.

	cout << "Simulation complete (or aborted): "
			 << "recording final state of system." << endl;

}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Commodity* Logician::getCommod(int ID)
{
	// Iterate through the Markets looking for the Commodity. Be sure to catch the 
	// GenExceptions thrown by the Markets that don't have it.
	vector<Market*>::iterator iter;
	for (iter = markets.begin(); iter != markets.end(); iter ++) {
		try {
			return (*iter)->getCommod(ID);
		}
		catch(GenException ge) {}
	}

	// If we didn't find it, throw a GenException of our own.
	throw GenException("Error: tried to access Commodity not loaded in the Logician.");
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Commodity* Logician::getCommod(string name)
{
	// Iterate through the Markets looking for the Commodity. Be sure to catch the 
	// GenExceptions thrown by the Markets that don't have it.
	vector<Market*>::iterator iter;
	for (iter = markets.begin(); iter != markets.end(); iter ++) {
		try {
			return (*iter)->getCommod(name);
		}
		catch(GenException ge) {}
	}

	// If we didn't find it, throw a GenException of our own.
	throw GenException("Error: tried to access Commodity not loaded in the Logician.");
}
