#if !defined(_STUBMARKET_H)
#define _STUBMARKET_H
#include <iostream>
#include "MarketModel.h"

using namespace std;

/**
 * The StubMarket class inherits from the MarketModel class and is dynamically
 * loaded by the Model class when requested.
 * 
 * This market will do nothing. This MarketModel is intended as a skeleton to guide
 * the implementation of new MarketModel models. 
 *
 */
class StubMarket : public MarketModel  
{
public:
        StubMarket() {};
  
	/**
	 * Constructs a StubMarket with specified data.
	 * and currently prints the sum.
	 *
	 * @param name is a string, the name of the market
	 */
         StubMarket(string market_name)
	     : MarketModel(market_name) {};

	StubMarket(string market_name, istream &input);

        ~StubMarket() {};

	/**
	 * prints the name of this mktType
	 *
	 */
	virtual void print();

    /// get model implementation name
    virtual const string getModelName() { return "StubMarket"; };

protected: 



};

extern "C" Model* construct(string market_name,istream &input) {
    return new StubMarket(market_name,input);
}

extern "C" void destruct(Model* p) {
    delete p;
}
#endif
