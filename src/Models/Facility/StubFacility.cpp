// StubFacility.cpp
// Implements the StubFacility class
#include <iostream>

#include "StubFacility.h"

#include "Logician.h"
#include "GenException.h"
#include "InputXML.h"

StubFacility::StubFacility(xmlNodePtr cur)
    : FacilityModel(cur) // assign ID & increment
{

    /// skip the rest for now
}


void StubFacility::print() 
{ 
    cout << "This is a StubFacility model with name " << name 
	 << " and ID: " << ID << " that converts commodities {";
    for (vector<Commodity*>::iterator commod=in_commods.begin();
 	 commod != in_commods.end();)
    {
	cout << (*commod)->getName();
	cout << (++commod != in_commods.end() ? "," : "" );
    }
    cout << "} into commodities {";
    for (vector<Commodity*>::iterator commod=out_commods.begin();
	 commod != out_commods.end();)
    {
	cout << (*commod)->getName();
	cout << (++commod != out_commods.end() ? "," : "" );
    }
    cout << "}" << endl;
    
};






