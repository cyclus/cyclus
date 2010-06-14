// Commodity.cpp
#include <string>
#include "Commodity.h"

//using namespace std;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Commodity::Commodity(int ID, string name, Market* mkt, bool fissile, bool sepMat){
	myID = ID;
	nextID = ID +1;
	myName = name;
  isFissile = fissile;
	isSepMat = sepMat;
	marketPtr = mkt;
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Commodity::~Commodity(){
	// Delete any commodity data members that remain even when 
	// we go out of Commodity scope... unless you want to write 
	// that info to the database.
};


