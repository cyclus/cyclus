// Inst.cpp
// Implements Inst class.
#include "Inst.h"
#include "FacFactory.h"
#include "GenException.h"
#include "Region.h"
#include <iostream>

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Inst::Inst(string s, int SN, Region* reg, double phi,int dur) 
	: name(s), ID(SN), grossPowerCap(0), annFixedChargeRt(phi),
		simDur(dur)
{
	build = map<string, vector<int> >();
	myFacs = vector<Facility*>();
	myRegion = reg;
	chargeRateLog = vector<double>(dur, 0.0);
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Inst::buildFac(string facModel, int time)
{	
	// Declare a pointer to the constructor
	FacFactory *newFacCreator = get_fac_map()[ facModel ];

	// Get a pointer to a new instance of the named 
	// Facility object of facModel type
	Facility *newFac = newFacCreator-> Create();

	// Push it.
	myFacs.push_back(newFac);
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Facility* Inst::getFac(int SN)
{
	vector<Facility*>::iterator iter;
	for (iter = myFacs.begin(); iter != myFacs.end(); iter ++)
		if ((*iter)->getSN() == SN)
			return *iter;
	throw GenException("Error: tried to access Fac not present in this Inst");
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const string Inst::getName() const
{
	return name;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int Inst::getSN() const
{
	return ID;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Region* Inst::getRegion()
{
	return myRegion;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Inst::handleTick(int time)
{
	// Build Facilities.

	// Iterate over the build map and build whatever's there at this timestep.
	map<string, vector<int> >::iterator biter;
	for (biter = build.begin(); biter != build.end(); biter ++) {

		string model = biter->first; // what kind of Facility?
		int num = biter->second[time]; // how many?

		for (int i = 0; i < num; i ++ )  // build 'em
			buildFac(model, time);
	}

	// Pass the tick to this Institution's Facilities.
	vector<Facility*>::iterator iter;
	iter = myFacs.begin();
	while (iter != myFacs.end()) {
		(*iter)->handleTick(time);
		iter ++;
	}
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Inst::handleTock(int time)
{
	// Pass the tock to this Institution's Facilities.
	vector<Facility*>::iterator iter;
	iter = myFacs.begin();
	while (iter != myFacs.end()) {
		(*iter)->handleTock(time);
		iter ++;
	}

	// Record this Institution's current annual fixed charge rate.
	chargeRateLog[time] = annFixedChargeRt;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
vector<double>* Inst::getChargeRateLog() {
	return &chargeRateLog;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/*
 * Let's not figure out the message class tonight.
 *
 * void Inst::receiveMessage(Message* theMessage) {
	Communicator* sender = theMessage->getSender();
	Communicator* me = (Communicator*) this;
	if (&sender == &me)
		;// placeholder!!
	else if (theMessage->getDir() == up)
		myRegion->receiveMessage(theMessage);
	else {
		vector<Facility*>::iterator iter;
		iter = myFacs.begin();
		while (iter != myFacs.end()) {
			(*iter)->receiveMessage(theMessage);
			iter ++;
		}
	}
}
*/
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Inst::~Inst()
{
	// Delete the Facilities.
	while(!myFacs.empty()) {
		Facility* f = myFacs.back();
		myFacs.pop_back();
		delete f;
	}
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Inst::handleEnd(int time)
{
	// Perform any Institution-level end-of-simulation bookkeeping.

	// Log this Institution.
	//BI->writeInst(this);

	// Pass the handleEnd() message to this Institution's Facilities.
	vector<Facility*>::iterator iter;
	iter = myFacs.begin();
	while (iter != myFacs.end()) {
		(*iter)->handleEnd(time);
		iter ++;
	}
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Inst::addFac(Facility* f)
{
	myFacs.push_back(f);
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Inst::setBuild(string facModel, vector<int> v)
{
	build[facModel] = v;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream& operator<<(ostream &os, const Inst& i)
{
	os << i.name << endl;
	return os;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream& operator<<(ostream &os, const Inst* i)
{
	os << *i;
	return os;
}
