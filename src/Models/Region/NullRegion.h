// NullRegion.h
#if !defined(_NULLREGION_H)
#define _NULLREGION_H
#include <iostream>

#include "RegionModel.h"

/**
 * The NullRegion class inherits from the RegionModel class and is dynamically
 * loaded by the Model class when requested.
 * 
 * This region will do nothing. This RegionModel never alters any messages
 * transmitted through it or anything else. 
 */
class NullRegion : public RegionModel  
{
/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

  public:
    /**
     * The default constructor for the NullRegion
     */
    NullRegion() {};

    /**
     * The default destructor for the NullRegion
     */
    ~NullRegion() {};
   
    // different ways to populate an object after creation
    /// initialize an object from XML input
    virtual void init(xmlNodePtr cur)  { RegionModel::init(cur); };

    /// initialize an object by copying another
    virtual void copy(NullRegion* src) { RegionModel::copy(src); } ;

    /**
     * This drills down the dependency tree to initialize all relevant parameters/containers.
     *
     * Note that this function must be defined only in the specfici model in question and not in any 
     * inherited models preceding it.
     *
     * @param src the pointer to the original (initialized ?) model to be copied
     */
    virtual void copyFreshModel(Model* src){ copy((NullRegion*)src); };
  
    // print information about the region
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
    return new NullRegion();
}

extern "C" void destruct(Model* p) {
    delete p;
}

/* -------------------- */

#endif
