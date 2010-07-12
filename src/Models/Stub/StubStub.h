// StubStub.h
#if !defined(_STUBSTUB_H)
#define _STUBSTUB_H
#include <iostream>

#include "StubModel.h"

/**
 * The StubStub class inherits from the StubModel class and is dynamically
 * loaded by the Model class when requested.
 * 
 * This model will do nothing. This StubModel is intended as a skeleton to guide
 * the implementation of new StubStub models. 
 *
 */
class StubStub : public StubModel  
{
/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

public:
    StubStub() {};
    
    StubStub(xmlNodePtr cur);
    
    ~StubStub() {};
   
    virtual void print();

    /// get model implementation name
    virtual const string getModelName() { return "StubStub"; };

/* ------------------- */ 

/* --------------------
 * all STUBMODEL classes have these members
 * --------------------
 */

/* ------------------- */ 

};

/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

extern "C" Model* construct(xmlNodePtr cur) {
    return new StubStub(cur);
}

extern "C" void destruct(Model* p) {
    delete p;
}

/* -------------------- */

#endif
