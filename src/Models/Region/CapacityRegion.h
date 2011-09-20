// CapacityRegion.h
#if !defined(_CAPACITYREGION_H)
#define _CAPACITYREGION_H

/**
 * The CapacityRegion class inherits from the RegionModel class and is dynamically
 * loaded by the Model class when requested.
 * 
 * This region will build new facilities based on the pleaseBuild() function. 
 * This RegionModel never alters any messages transmitted through it or anything else. 
 */

#include "RegionModel.h"
#include "Logician.h"
#include "GenException.h"
#include "InputXML.h"

#include <queue>
#include <utility>
#include <vector>

typedef std::vector <Model*> ReplacementFacs;
typedef std::vector <ReplacementFacs> AllReplacementFacs;

class CapacityRegion : public RegionModel  
{
/* --------------------
 * The CapacityRegion has the following initial functions
 * --------------------
 */

  private:
    /**
     * Populate the build schedule
     */
  void populateSchedule(FILE *infile);

/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

  public:
    /**
     * The default constructor for the CapacityRegion
     */
    CapacityRegion() {};

    /**
     * The default destructor for the CapacityRegion
     */
    ~CapacityRegion() {};
   
    // different ways to populate an object after creation
    /// initialize an object from XML input
    virtual void init(xmlNodePtr cur);

    /// initialize an object by copying another
    virtual void copy(CapacityRegion* src) { RegionModel::copy(src); } ;

    /**
     * This drills down the dependency tree to initialize all relevant parameters/containers.
     *
     * Note that this function must be defined only in the specific model in question and not in any 
     * inherited models preceding it.
     *
     * @param src the pointer to the original (initialized ?) model to be copied
     */
    virtual void copyFreshModel(Model* src){ copy(dynamic_cast<CapacityRegion*>(src)); };
  
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

 protected:


/* -------------------- */

/* --------------------
 * the CapacityRegion class have these members
 * --------------------
 */

 public:
    bool requestBuild(Model* fac, InstModel* inst);
    int nFacs();
    string capacity_type(int i){return _capacity_type[i];};
    string capacity_function(int i){return _capacity_function[i];};
    double nominal_value(int i){return _nominal_value[i];};
    int nCapacities(){return _capacity_type.size();};

 protected:
    int _nFacs;
    std::vector <string> _capacity_type, _capacity_function;
    std::vector <double> _nominal_value;
    AllReplacementFacs allReplacementFacs;
    void initBuild(xmlNodePtr cur);
    void initCapacity(xmlNodePtr cur);
    Model* chooseInstToBuildFac();
    double checkCurrentCapcity(string capacity_type);
    Model* chooseFacToBuild(ReplacementFacs facs);
    map <string, queue <pair <int,int> > > to_build_map;
    map <string, queue <pair <int,int> > > have_built_map;
  
/* ------------------- */ 

};

/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

extern "C" Model* construct() {
    return new CapacityRegion();
}

extern "C" void destruct(Model* p) {
    delete p;
}

/* -------------------- */

#endif
