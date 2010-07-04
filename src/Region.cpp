// Region.cpp
// Implements the Region class

#include "Region.h"
#include "Facility.h"
#include "Logician.h"
#include "Inst.h"
// #include "Message.h"
#include "GenException.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Region::Region(RegionType type, string regName, int SN, int dur) : 
	myType(type), grossPowerCap(0.0), name(regName), ID(SN), simDur(dur)
{
	demand = vector<double>(dur, 0.0);
	myInsts = vector<Inst*>();
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Region::addInst(string s, int SN, double phi)
{
	Inst* newInst = new Inst(s, SN, this, phi, simDur);
	myInsts.push_back(newInst);
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Region::addInst(Inst* i)
{
	myInsts.push_back(i);
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Inst* Region::getInst(int SN)
{
	vector<Inst*>::iterator iter;
	for(iter = myInsts.begin(); iter != myInsts.end(); iter ++)
		if ((*iter)->getSN() == SN)
			return *iter;
	throw GenException("Error: tried to access Inst not present in this Region");
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Inst* Region::getInst(string name)
{
	vector<Inst*>::iterator iter;
	for(iter = myInsts.begin(); iter != myInsts.end(); iter ++)
		if ((*iter)->getName() == name)
			return *iter;
	throw GenException("Error: tried to access Inst not present in this Region");
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// vector<Inst*>* Region::getInsts()
// {
// 	return &myInsts;
// }
pair<vector<Inst*>::iterator, vector<Inst*>::iterator> Region::getInsts()
{
	return make_pair(myInsts.begin(), myInsts.end());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Region::setDemand(vector<double>& dem)
{
	demand = dem;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double Region::getDemand(int time)
{
	return demand[time];
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Region::handleTick(int time) {

	// Send the month tick to the Institutions.
	vector<Inst*>::iterator iter;
	iter = myInsts.begin();
	while (iter != myInsts.end()) {
		(*iter)->handleTick(time);
		iter ++;
	}

}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Region::handleTock(int time) {

	// Send the month tock to the Institutions.
	vector<Inst*>::iterator iter;
	iter = myInsts.begin();
	while (iter != myInsts.end()) {
		(*iter)->handleTock(time);
		iter ++;
	}
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
vector<double>* Region::getDemand() {
	return &demand;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/*
 * void Region::receiveMessage(Message* theMessage) {
	Communicator* sender = theMessage->getSender();
	Communicator* me = (Communicator*) this;
	if (&sender == &me)
		; // placeholder!!
	else if (theMessage->getDir() == up)
		MI->receiveMessage(theMessage);
	else {
		vector<Inst*>::iterator iter;
		iter = myInsts.begin();
		while (iter != myInsts.end()) {
			(*iter)->receiveMessage(theMessage);
			iter ++;
		}
	}
}
*/
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
RegionType Region::getType()
{

	// KDHFLAG what is the need for fuel cycle and non fuel cycle types? 

	return myType;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Region::~Region()
{
	// Delete the Insts first.
	while(!myInsts.empty()) {
		Inst* i = myInsts.back();
		myInsts.pop_back();
		delete i;
	}
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Region::handleEnd(int time) 
{
	// Write to the database.

	// BI->writeRegion(this);

	// Pass the message down.

	vector<Inst*>::iterator iter;
	iter = myInsts.begin();
	while (iter != myInsts.end()) {
		(*iter)->handleEnd(time);
		iter ++;
	}
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream& operator<<(ostream &os, const Region& r)
{
	os << r.name << endl;
	return os;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream& operator<<(ostream &os, const Region* r)
{
	os << *r;
	return os;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int Region::getSN() const
{
	return ID;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const string Region::getName() const
{
	return name;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int Region::getNumInsts() const
{
	return myInsts.size();
}
