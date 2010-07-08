// StubFacility.h
#if !defined(_STUBFACILITY_H)
#define _STUBFACILITY_H
#include <iostream>

#include "FacilityModel.h"

/**
 * The StubFacility class inherits from the FacilityModel class and is dynamically
 * loaded by the Model class when requested.
 * 
 * This facility will do nothing. This FacilityModel is intended as a skeleton to guide
 * the implementation of new FacilityModel models. 
 *
 */
class StubFacility : public FacilityModel  
{
/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

public:
    StubFacility() {};
    
    StubFacility(xmlNodePtr cur);

    ~StubFacility() {};

    virtual void print();

    /// get model implementation name
    virtual const string getModelName() { return "StubFacility"; };

/* ------------------- */ 

/* --------------------
 * all FACILITYMODEL classes have these members
 * --------------------
 */



/* ------------------- */ 
};

/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

extern "C" Model* construct(xmlNodePtr cur) {
    return new StubFacility(cur);
}

extern "C" void destruct(Model* p) {
    delete p;
}

/* ------------------- */ 

#endif
