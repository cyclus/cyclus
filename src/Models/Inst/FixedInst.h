// FixedInst.h
#if !defined(_FIXEDINST_H)
#define _FIXEDINST_H
#include <iostream>

#include "InstModel.h"

/**
 * The FixedInst class inherits from the InstModel class and is dynamically
 * loaded by the Model class when requested.
 * 
 * This InstModel defines an institution that has an initial set of facilities
 * that continue operating forever.  No facilities are added and no facilities
 * are decommissioned.
 *
 */
class FixedInst : public InstModel  
{
/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

public:
    FixedInst() {};
    
    ~FixedInst() {};
   
    // different ways to populate an object after creation
    /// initialize an object from XML input
    virtual void init(xmlNodePtr cur);
    /// initialize an object by copying another
    virtual void copy(FixedInst* src);

    virtual void print();

/* ------------------- */ 

/* --------------------
 * all COMMUNICATOR classes have these members
 * --------------------
 */
public:
    /// simply ignore incoming offers/requests.
    virtual void receiveOfferRequest(OfferRequest* msg) {};

protected:


/* -------------------- */

/* --------------------
 * all INSTMODEL classes have these members
 * --------------------
 */

/* ------------------- */ 

/* --------------------
 * This INSTMODEL classes have these members
 * --------------------
 */

protected:

    vector<Model*> facilities;

/* ------------------- */ 


};

/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

extern "C" Model* construct() {
    return new FixedInst();
}

extern "C" void destruct(Model* p) {
    delete p;
}

/* -------------------- */

#endif
