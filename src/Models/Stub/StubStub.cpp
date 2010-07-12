// StubStub.cpp
// Implements the StubStub class
#include <iostream>

#include "StubStub.h"

#include "Logician.h"
#include "GenException.h"
#include "InputXML.h"


StubStub::StubStub(xmlNodePtr cur)
    : StubModel(cur) // assign & incrememt ID
{


    /// skip rest for now
}

void StubStub::print() 
{ 
    cout << "This is a StubStub model with name " << name 
	 << " and ID: " << ID << endl;
};






