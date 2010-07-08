// StubRegion.h
#if !defined(_STUBREGION_H)
#define _STUBREGION_H
#include <iostream>

#include "RegionModel.h"

/**
 * The StubRegion class inherits from the RegionModel class and is dynamically
 * loaded by the Model class when requested.
 * 
 * This region will do nothing. This RegionModel is intended as a skeleton to guide
 * the implementation of new RegionModel models. 
 *
 */
class StubRegion : public RegionModel  
{
/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

public:
    StubRegion() {};
    
    StubRegion(xmlNodePtr cur);
    
    ~StubRegion() {};
   
    virtual void print();

    /// get model implementation name
    virtual const string getModelName() { return "StubRegion"; };

/* ------------------- */ 

/* --------------------
 * all REGIONMODEL classes have these members
 * --------------------
 */

/* ------------------- */ 

};

/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

extern "C" Model* construct(xmlNodePtr cur) {
    return new StubRegion(cur);
}

extern "C" void destruct(Model* p) {
    delete p;
}

/* -------------------- */

#endif
