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
    
    ~StubRegion() {};
   
    // different ways to populate an object after creation
    /// initialize an object from XML input
    virtual void init(xmlNodePtr cur)  { RegionModel::init(cur); };
    /// initialize an object by copying another
    virtual void copy(StubRegion* src) { RegionModel::copy(src); } ;

    virtual void print()               { RegionModel::print();   } ;

/* ------------------- */ 

/* --------------------
 * all COMMUNICATOR classes have these members
 * --------------------
 */
public:
    
protected:


/* -------------------- */

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

extern "C" Model* construct() {
    return new StubRegion();
}

extern "C" void destruct(Model* p) {
    delete p;
}

/* -------------------- */

#endif
