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
    
    ~StubStub() {};
   
    // different ways to populate an object after creation
    /// initialize an object from XML input
    virtual void init(xmlNodePtr cur) { StubModel::init(cur); };
    /// initialize an object by copying another
    virtual void copy(StubStub* src)  { StubModel::copy(src); } ;

    virtual void print()              { StubModel::print();   } ;

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

extern "C" Model* construct() {
    return new StubStub();
}

extern "C" void destruct(Model* p) {
    delete p;
}

/* -------------------- */

#endif
