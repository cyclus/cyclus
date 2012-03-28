// BuildRegion.h
#if !defined(_BUILDREGION_H)
#define _BUILDREGION_H

/**
   The BuildRegion class inherits from the RegionModel class and is dynamically
   loaded by the Model class when requested.
   
   This region will build new facilities based on the pleaseBuild() function. 
   This RegionModel never alters any messages transmitted through it or anything else. 
 */

#include "RegionModel.h"
#include "InstModel.h"
#include "CycException.h"
#include "InputXML.h"

#include <queue>
#include <utility>

class BuildRegion : public RegionModel  
{
/* --------------------
 * The BuildRegion has the following initial functions
 * --------------------
 */

  private:
    /**
       Populate the build schedule
     */
  void populateSchedule(FILE *infile);

/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

  public:
    /**
       The default constructor for the BuildRegion
     */
    BuildRegion() {};

    /**
       The default destructor for the BuildRegion
     */
    virtual ~BuildRegion() {};
   
    // different ways to populate an object after creation
    /// initialize an object from XML input
    virtual void init(xmlNodePtr cur);

    /// initialize an object by copying another
    virtual void copy(BuildRegion* src) { RegionModel::copy(src); } ;

    /**
       This drills down the dependency tree to initialize all relevant parameters/containers.
       
       Note that this function must be defined only in the specific model in question and not in any 
       inherited models preceding it.
       
       @param src the pointer to the original (initialized ?) model to be copied
     */
    virtual void copyFreshModel(Model* src){ copy(dynamic_cast<BuildRegion*>(src)); };
  
    // print information about the region
    virtual void print()               { RegionModel::print();   } ;

/* ------------------- */ 

/* --------------------
 * all COMMUNICATOR classes have these members
 * --------------------
 */
 public:
    
    /**
       Each region is prompted to do its beginning-of-time-step
       stuff at the tick of the timer.
       The default behavior is to ignore the tick.
       
       @param time is the time to perform the tick
     */
    virtual void handleTick(int time);

 protected:


/* -------------------- */

/* --------------------
 * the BuildRegion class have these members
 * --------------------
 */

 public:
    bool requestBuild(Model* fac, InstModel* inst);
    int nFacs();

 protected:
    int nFacs_;
    Model* chooseInstToBuildFac();
    map <string, queue <pair <int,int> > > to_build_map_;
    map <string, queue <pair <int,int> > > have_built_map_;
  
/* ------------------- */ 

/* --------------------
   output directory info
 * --------------------
 */
 public:
  /**
     The getter function for this region model's output dir
  */
  static std::string outputDir(){ 
    return RegionModel::outputDir().append(outputDir_);}
    
 private:
  /**
     Every specific region model writes to the output database
     location: RegionModel::OutputDir_ + /this_region's_name
  */
  static std::string outputDir_;

};

#endif
