// StubMarket.cpp
// Implements the StubMarket class
#include <iostream>
#include "StubMarket.h"
#include "Logician.h"
#include "GenException.h"

using namespace std;


StubMarket::StubMarket(string market_name, istream &input)
    : MarketModel(market_name)
{

    string keyword;

    /// all markets need a commodity
    input >> keyword;

    if (keyword != "Commodity")
	throw GenException("Missing commodity in market");
    
    input >> keyword;

    commodity = LI->addCommodity(keyword,input,this);

}

void StubMarket::print() 
{ 
    cout << "This is a StubMarket model with name " << name 
	 << " and ID: " << ID << " that trades commodity " << commodity->getName() << endl;
}


