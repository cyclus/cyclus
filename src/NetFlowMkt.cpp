#include <iostream>
#include "NetFlowMkt.h"

void NetFlowMkt::printMyName() 
{ 
	cout << "My Name : NetFlowMkt and bob is "<< bob << endl;
};


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
NetFlowMkt::NetFlowMkt() : Market()
{	
	bob = 0;
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
NetFlowMkt::NetFlowMkt(double a, double b) : Market()
{	
	bob = a+b;
};

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
			return new NetFlowMkt();
    }
  }mktType;

  class netFlowCreator2 : public MktFactory
  {
		public:
     netFlowCreator2()
     {
        get_mkt_map()["netFlow2"]=this;
		 }
    virtual Market * Create()const
    {
			double a,b;
			cout << "\n" << "give me an a: "; cin >> a; 
			cout << "\n" << "give me a b: "; cin >> b; 
			return new NetFlowMkt(a,b);
    }
  }mktType2;

}

//namespace {
//     MktFactoryConcrete<NetFlowMkt> facType("null");
//}


