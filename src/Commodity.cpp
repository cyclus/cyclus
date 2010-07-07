// Commodity.cpp
// Implements the Commodity Class
#include <string>
#include "Logician.h"
#include "Commodity.h"
#include "GenException.h"


Commodity::Commodity(string commod_name, Model* my_market, istream &input)
    : name(commod_name), market(my_market)
{
    ID = nextID++;
    
    /// for now commodities have no input
}

Commodity::~Commodity(){
	// Delete any commodity data members that remain even when 
	// we go out of Commodity scope... unless you want to write 
	// that info to the database.
}

/// Initialize the commodity ID serialization
int Commodity::nextID = 0;

