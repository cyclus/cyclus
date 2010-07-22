// StubStubComm.h
#if !defined(_STUBSTUBCOMM_H)
#define _STUBSTUBCOMM_H
#include <iostream>

#include "StubCommModel.h"

/**
 * The StubStubComm class inherits from the StubModel class and is dynamically
 * loaded by the Model class when requested.
 * 
 * This model will do nothing. This StubCommModel is intended as a skeleton to guide
 * the implementation of new StubComm models. 
 *
 */
class StubStubComm : public StubCommModel  
{
/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

public:
    StubStubComm() {};
    
    ~StubStubComm() {};
   
    // different ways to populate an object after creation
    /// initialize an object from XML input
    virtual void init(xmlNodePtr cur)    { StubCommModel::init(cur); };
    /// initialize an object by copying another
    virtual void copy(StubStubComm* src) { StubCommModel::copy(src); } ;

    virtual void print()                 { StubCommModel::print();   } ;

/* ------------------- */ 

/* --------------------
 * all COMMUNICATOR classes have these members
 * --------------------
 */
public:
    /// never generate any messages
    virtual void sendOfferRequest() {};
    /// Simply ingore incoming messages
    virtual void receiveOfferRequest(OfferRequest* msg) {};


protected:

/* -------------------- */

/* --------------------
 * all STUBCOMMMODEL classes have these members
 * --------------------
 */

/* ------------------- */ 

};

/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

extern "C" Model* construct() {
    return new StubStubComm();
}

extern "C" void destruct(Model* p) {
    delete p;
}

/* -------------------- */

#endif
