#if !defined(_NETFLOWMKT_H)
#define _NETFLOWMKT_H
#include <iostream>
#include "MarketModel.h"

using namespace std;

/**
 * The NetFlowMkt class inherits from the MarketModel class and is dynamically
 * loaded by the Model class when requested.
 * 
 * This market will do nothing. This MarketModel is intended as a skeleton to guide
 * the implementation of new MarketModel models. 
 *
 */
class NetFlowMkt : public MarketModel  
{
public:
        NetFlowMkt() {};
  
	/**
	 * Constructs a NetFlowMkt with specified data.
	 * and currently prints the sum.
	 *
	 * @param name is a string, the name of the market
	 */
         NetFlowMkt(string market_name)
	     : MarketModel(market_name) {};

	NetFlowMkt(string market_name, istream &input);

        ~NetFlowMkt() {};

	/**
	 * prints the name of this mktType
	 *
	 */
	virtual void print();

    /// get model implementation name
    virtual const string getModelName() { return "NetFlowMkt"; };

protected: 



};

extern "C" Model* construct(string market_name,istream &input) {
    return new NetFlowMkt(market_name, input);
}

extern "C" void destruct(Model* p) {
    delete p;
}
#endif
