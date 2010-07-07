// StubFacility.cpp
// Implements the StubFacility class
#include <iostream>

#include "StubFacility.h"

#include "Logician.h"
#include "GenException.h"

StubFacility::StubFacility(string facility_name,istream &input)
    : FacilityModel(facility_name)
{
    string keyword;

    /// all facilities need an input commodity
    input >> keyword;

    if (keyword != "InCommodity")
	throw GenException("Need single input commodity for facility.");
    
    input >> keyword;

    in_commods.push_back(LI->addCommodity(keyword,input));

    /// all facilities need an ouput commodity
    input >> keyword;

    if (keyword != "OutCommodity")
	throw GenException("Need single output commodity for facility.");
    
    input >> keyword;

    out_commods.push_back(LI->addCommodity(keyword,input));
    

}


void StubFacility::print() 
{ 
    cout << "This is a StubFacility model with name " << name 
	 << " and ID: " << ID << " that converts commodities {";
    for (vector<Commodity*>::iterator commod=in_commods.begin();
	 commod != in_commods.end();)
    {
	cout << (*commod)->getName();
	cout << (++commod != in_commods.end() ? "," : "}" );
    }
    cout << " into commodities {";
    for (vector<Commodity*>::iterator commod=out_commods.begin();
	 commod != out_commods.end();)
    {
	cout << (*commod)->getName();
	cout << (++commod != out_commods.end() ? "," : "}" );
    }
    cout << endl;
    
};






