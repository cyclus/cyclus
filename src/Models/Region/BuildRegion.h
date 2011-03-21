// NullRegion.h
#if !defined(_BUILDREGION_H)
#define _BUILDREGION_H
#include <iostream>

#include "RegionModel.h"

/**
 * The BuildRegion class inherits from the RegionModel class and is dynamically
 * loaded by the Model class when requested.
 * 
 * This region will build new facilities based on the build() function. 
 * This RegionModel never alters any messages transmitted through it or anything else. 
 */
class BuildRegion : public RegionModel  
{
 public:
  /**
   * The build function.
   */
  void build();

/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

  public:
    /**
     * The default constructor for the BuildRegion
     */
    BuildRegion() {};

    /**
     * The default destructor for the BuildRegion
     */
    ~BuildRegion() {};
   
    // different ways to populate an object after creation
    /// initialize an object from XML input
    virtual void init(xmlNodePtr cur)  { RegionModel::init(cur); };

    /// initialize an object by copying another
    virtual void copy(BuildRegion* src) { RegionModel::copy(src); } ;

    /**
     * This drills down the dependency tree to initialize all relevant parameters/containers.
     *
     * Note that this function must be defined only in the specific model in question and not in any 
     * inherited models preceding it.
     *
     * @param src the pointer to the original (initialized ?) model to be copied
     */
    virtual void copyFreshModel(Model* src){ copy((BuildRegion*)src); };
  
    // print information about the region
    virtual void print()               { RegionModel::print();   } ;

/* ------------------- */ 

/* --------------------
 * all COMMUNICATOR classes have these members
 * --------------------
 */
 public:
    
    /**
     * Each region is prompted to do its beginning-of-time-step
     * stuff at the tick of the timer.
     * The default behavior is to ignore the tick.
     *
     * @param time is the time to perform the tick
     */
    virtual void handleTick(int time);

    /**
     * Each region is prompted to do its beginning-of-time-step
     * stuff at the tock of the timer.
     * The default behavior is to ignore the tock.
     *
     * @param time is the time to perform the tock
     */
    virtual void handleTock(int time);
    
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
    return new BuildRegion();
}

extern "C" void destruct(Model* p) {
    delete p;
}

/* -------------------- */

#endif
