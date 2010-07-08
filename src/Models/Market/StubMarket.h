// StubMarket.h
#if !defined(_STUBMARKET_H)
#define _STUBMARKET_H
#include <iostream>

#include "MarketModel.h"

/**
 * The StubMarket class inherits from the MarketModel class and is dynamically
 * loaded by the Model class when requested.
 * 
 * This market will do nothing. This MarketModel is intended as a skeleton to
 * guide the implementation of new MarketModel models.
 *
 */
class StubMarket : public MarketModel  
{
/* --------------------
 * all MODEL classes have these public members
 * --------------------
 */

public:
    StubMarket() {};
    
    StubMarket(xmlNodePtr cur);
    
    ~StubMarket() {};
    
    virtual void print();
    
    /// get model implementation name
    virtual const string getModelName() { return "StubMarket"; };

/* -------------------- */

/* --------------------
 * all MARKETMODEL classes have these members
 * --------------------
 */

/* -------------------- */

/* --------------------
 * _THIS_ MARKETMODEL class these members
 * --------------------
 */

/* -------------------- */

};

/* --------------------
 * all MODEL classes have these members
 * --------------------
 */
extern "C" Model* construct(xmlNodePtr cur) {
    return new StubMarket(cur);
}

extern "C" void destruct(Model* p) {
    delete p;
}

/* -------------------- */

#endif
