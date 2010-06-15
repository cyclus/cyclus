// Commodity.cpp
#include <string>
#include "Commodity.h"
#include "GenException.h"

//using namespace std;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Commodity::Commodity(string name, int ID, Market* mkt, bool fissile, bool sepMat){
	myID = ID;
	nextID = ID +1;
	myName = name;
  isFissile = fissile;
	isSepMat = sepMat;
	myMarket = mkt;
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
Market* Commodity::getMarket()
{
	return myMarket;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Commodity::~Commodity(){
	// Delete any commodity data members that remain even when 
	// we go out of Commodity scope... unless you want to write 
	// that info to the database.
};


