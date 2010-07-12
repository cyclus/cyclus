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
    
    StubInst(xmlNodePtr cur);
    
    ~StubInst() {};
   
    virtual void print();

    /// get model implementation name
    virtual const string getModelName() { return "StubInst"; };

/* ------------------- */ 

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
    map<int,Model*> deployment_map;

/* ------------------- */ 


};

/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

extern "C" Model* construct(xmlNodePtr cur) {
    return new StubInst(cur);
}

extern "C" void destruct(Model* p) {
    delete p;
}

/* -------------------- */

#endif
