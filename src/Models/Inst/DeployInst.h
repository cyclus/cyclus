// Deployinst.h
#if !defined(_DEPLOYINST_H)
#define _DEPLOYINST_H
#include <iostream>

#include "InstModel.h"

/**
 * The DeployInst class inherits from the InstModel class and is dynamically
 * loaded by the Model class when requested.
 * 
 * This model implements a simple institution model that deploys specific
 * facilities as defined explicitly in the input file.
 *
 */
class DeployInst : public InstModel  
{
/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

public:
    DeployInst() {};
    
    DeployInst(xmlNodePtr cur);
    
    ~DeployInst() {};
   
    virtual void print();

    /// get model implementation name
    virtual const string getModelName() { return "DeployInst"; };

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
    return new DeployInst(cur);
}

extern "C" void destruct(Model* p) {
    delete p;
}

/* -------------------- */

#endif
