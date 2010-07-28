// StubInst.h
#if !defined(_STUBINST_H)
#define _STUBINST_H
#include <iostream>

#include "InstModel.h"

/**
 * The StubInst class inherits from the InstModel class and is dynamically
 * loaded by the Model class when requested.
 * 
 * This model will do nothing. This InstModel is intended as a skeleton to guide
 * the implementation of new InstModel models. 
 *
 */
class StubInst : public InstModel  
{
/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

public:
    StubInst() {};
    
    ~StubInst() {};
   
    // different ways to populate an object after creation
    /// initialize an object from XML input
    virtual void init(xmlNodePtr cur) { InstModel::init(cur); };
    /// initialize an object by copying another
    virtual void copy(StubInst* src)  { InstModel::copy(src); } ;

    virtual void print()              { InstModel::print(); cout << endl;  } ;

/* ------------------- */ 

/* --------------------
 * all COMMUNICATOR classes have these members
 * --------------------
 */
public:
    /// simply ignore incoming offers/requests.
    virtual void receiveMessage(Message* msg) {};

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

/* ------------------- */ 


};

/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

extern "C" Model* construct() {
    return new StubInst();
}

extern "C" void destruct(Model* p) {
    delete p;
}

/* -------------------- */

#endif
