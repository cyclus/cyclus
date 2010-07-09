// StubMarket.cpp
// Implements the StubMarket class
#include <iostream>

#include "StubMarket.h"

#include "Logician.h"
#include "GenException.h"
#include "InputXML.h"


StubMarket::StubMarket(xmlNodePtr cur)
    : MarketModel(cur) // assign ID & increment
{

    /// process parameters
    
}

void StubMarket::print() 
{ 
    cout << "This is a StubMarket model with name " << name 
	 << " and ID: " << ID << " and trading commodity " 
	 << commodity->getName() << endl;
}


