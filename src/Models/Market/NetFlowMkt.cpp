// NetFlowMkt.cpp
// Implements the NetFlowMkt class
#include <iostream>
#include "NetFlowMkt.h"

using namespace std;


NetFlowMkt::NetFlowMkt(string market_name, istream &input)
    : MarketModel(market_name)
{

    /// this model has no other input so do nothing

}

void NetFlowMkt::print() 
{ 
    cout << "This is a NetFlowMkt model with name " << name 
	 << " and ID: " << ID << endl;
}


