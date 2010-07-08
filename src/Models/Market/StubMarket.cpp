// StubMarket.cpp
// Implements the StubMarket class
#include <iostream>

#include "StubMarket.h"

#include "Logician.h"
#include "GenException.h"
#include "InputXML.h"


StubMarket::StubMarket(xmlNodePtr cur)
    : MarketModel() // assign ID & increment
{
    name = (const char*)xmlGetProp(cur, (const xmlChar*)"name");

    /// skip the rest for now
}

void StubMarket::print() 
{ 
    cout << "This is a StubMarket model with name " << name 
	 << " and ID: " << ID << endl;
}


