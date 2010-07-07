// Commodity.cpp
// Implements the Commodity Class
#include <string>
#include "Logician.h"
#include "Commodity.h"
#include "GenException.h"


// Initialize the commodity ID serialization
int Commodity::nextID = 0;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Commodity::Commodity(string name, Market* mkt, bool fiss, bool sep){
	myID = this->getNextID();
	myName = name;
  fissile = fiss;
	sepMat = sep;
	myMarket = mkt;
	myMarket->addCommod(this);
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Commodity::~Commodity(){
	// Delete any commodity data members that remain even when 
	// we go out of Commodity scope... unless you want to write 
	// that info to the database.
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const string Commodity::getName() const
{
	return myName;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int Commodity::getSN() const
{
	return myID;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool Commodity::isFissile() const
{
	return fissile;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool Commodity::isSepMat() const
{
	return sepMat;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int Commodity::getNextID()
{
	nextID++;
	return nextID;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Market* Commodity::getMarket()
{
	return myMarket;
}
