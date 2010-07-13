// StubStubComm.cpp
// Implements the StubStubComm class
#include <iostream>

#include "StubStubComm.h"

#include "Logician.h"
#include "GenException.h"
#include "InputXML.h"


StubStubComm::StubStubComm(xmlNodePtr cur)
    : StubCommModel(cur) // assign & incrememt ID
{


    /// skip rest for now
}

void StubStubComm::print() 
{ 
    cout << "This is a StubStubComm model with name " << name 
	 << " and ID: " << ID << endl;
};






