// StubRegion.cpp
// Implements the StubRegion class
#include <iostream>

#include "StubRegion.h"

#include "GenException.h"
#include "Logician.h"

StubRegion::StubRegion(string region_name, istream &input)
    : RegionModel(region_name)
{

    string token;

    input >> token;

    if (token != "AllowedFacility")
	throw GenException("Need to define allowed facilities for this region.");

    input >> token;

    Model* new_facility = LI->getFacilityByName(token);

    if (NULL != new_facility)
	allowedFacilities.push_back(new_facility);

}

void StubRegion::print() 
{ 
    cout << "This is a StubRegion model with name " << name 
	 << " and ID: " << ID << " with allowed facility " << allowedFacilities[0]->getName() <<  endl;
};






