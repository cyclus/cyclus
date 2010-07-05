// NetFlowMkt.cpp
// Implements the NetFlowMkt class
#include <iostream>
#include "NetFlowMkt.h"
#include "InputDB.h"

using namespace std;


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
NetFlowMkt::NetFlowMkt(string name, int SN) : Market(name, SN)
{	
	this->registerMkt();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void NetFlowMkt::printMyName() 
{ 
	cout << "My Name : NetFlowMkt" << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
namespace {
  class netFlowCreator : public MktFactory
  {
		public:
     netFlowCreator()
     {
        get_mkt_map()["netFlow"]=this;
		 }
    virtual Market * Create()const
    {
			int myID = Market::getNextID();
			string myName = INDB->getMktName(myID);
			return new NetFlowMkt(myName, myID);
    }
  }mktType;

}

