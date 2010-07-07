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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Logician::Logician() {
	facilities = vector<Facility*>();
	markets = vector<Market*>();
	mktMap = MarketMap();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Logician::~Logician()
{
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Logician::receiveMessage(Message* theMessage)
{
	string msgDir = theMessage->Message::unEnumerateDir();
	double msgAmount = theMessage->Message::getAmount();
	double msgPrice = theMessage->Message::getPrice();
	Commodity* msgCommod = theMessage->Message::getCommod();
	string strCommod = msgCommod->getName();
	int msgFacID = theMessage->Message::getFacID();

	cout << " -------------MessageTest ------------" << endl;
	cout << "The Logician has received a message" << endl;
	cout << "  Its direction is : " << msgDir << endl;
	cout << "  The amount requested/offered is : " << msgAmount << endl;
	cout << "  Its price is : " << msgPrice << endl;
	cout << "  Its commodity is : " << strCommod << endl;
	cout << "  It originated from Facility : " << msgFacID << endl;
	cout << "--------------------------------------" << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Logician* Logician::Instance() {
	// If we haven't created a Logician yet, create and return it.
	if (0 == _instance)
		_instance = new Logician();
	
	return _instance;
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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Logician::setSimDur(int numMonths) {
	simDur = numMonths;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Logician::addMarket(Market* newcomer){
	markets.push_back(newcomer);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Logician::handleTick(int time) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Logician::handleTock(int time) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Logician::handleEnd(int time)
{
	// Perform any final logician tasks.

	cout << "Simulation complete (or aborted): "
			 << "recording final state of system." << endl;

}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pair<vector<Facility*>::iterator, vector<Facility*>::iterator> Logician::getFacilities()
{
	return make_pair(facilities.begin(), facilities.end());
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pair<vector<Market*>::iterator, vector<Market*>::iterator> Logician::getMarkets()
{
	return make_pair(markets.begin(), markets.end());
}
