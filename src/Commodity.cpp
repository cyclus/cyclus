// Commodity.cpp
// Implements the Commodity Class
#include <string>
#include "Logician.h"
#include "Commodity.h"
#include "GenException.h"


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
Market* Commodity::getMarket()
{
	return myMarket;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Commodity::~Commodity(){
	// Delete any commodity data members that remain even when 
	// we go out of Commodity scope... unless you want to write 
	// that info to the database.
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int Commodity::getNextID()
{
	nextID++;
	return nextID;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

int Commodity::nextID = 0;
// Initialize the commodity ID serialization

