// StubRegion.cpp
// Implements the StubRegion class
#include <iostream>

#include "StubRegion.h"

#include "Logician.h"
#include "GenException.h"
#include "InputXML.h"


StubRegion::StubRegion(xmlNodePtr cur)
    : RegionModel(cur) // assign & incrememt ID
{


    /// skip rest for now
}

void StubRegion::print() 
{ 
    cout << "This is a StubRegion model with name " << name 
	 << " and ID: " << ID << endl;

    for(vector<Model*>::iterator inst=institutions.begin();
	inst != institutions.end();
	inst++)
	(*inst)->print();
};






