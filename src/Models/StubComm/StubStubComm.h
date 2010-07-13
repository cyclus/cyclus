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
    
    StubStubComm(xmlNodePtr cur);
    
    ~StubStubComm() {};
   
    virtual void print();

    /// get model implementation name
    virtual const string getModelName() { return "StubStubComm"; };

/* ------------------- */ 

/* --------------------
 * all COMMUNICATOR classes have these members
 * --------------------
 */
public:
    /// Simply ingore incoming messages
    virtual void sendOfferRequest();
    virtual void receiveOfferRequest(OfferRequest* msg) {};
    virtual void transmitOfferRequest(OfferRequest* msg) ;

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

extern "C" Model* construct(xmlNodePtr cur) {
    return new StubStubComm(cur);
}

extern "C" void destruct(Model* p) {
    delete p;
}

/* -------------------- */

#endif
