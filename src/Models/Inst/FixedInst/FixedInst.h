// FixedInst.h
#if !defined(_FIXEDINST_H)
#define _FIXEDINST_H

#include <queue>

#include "InstModel.h"

#include "Logger.h"

/**
   @class FixedInst 
    
   @section introduction Introduction 
   The FixedInst is an institution type in Cyclus which is 
   unchanging and is statically associated with facilities that are 
   deployed by some other entity (such as the region or the 
   logician). The FixedInst is instantiated at the beginning of the 
   simulation and exists until the end of the simulation. 
    
   @section detailedBehavior Detailed Behavior 
   The FixedInst starts operation at the beginning of the 
   simulation and ends operation at the end of the simulation. It 
   populates its list of facilities when it is initialized and 
   determines its region as all Institution models do. When it 
   receives a message, it transmits that message immediately up to 
   its region or down to the appropriate facility without making 
   any changes. 
 */

class FixedInst : public InstModel {
/* --------------------
 * all MODEL classes have these members
 * --------------------
 */
 public:
  /**
     Default constructor for the fixed inst 
   */
  FixedInst() {};
    
  /**
     Default destructor for the fixed inst 
   */
  virtual ~FixedInst() {};
  
  // different ways to populate an object after creation
  /**
     initialize an object from XML input 
   */
  virtual void init(xmlNodePtr cur);

  /**
     initialize an object by copying another 
   */
  virtual void copy(FixedInst* src);

  /**
     This drills down the dependency tree to initialize all relevant 
     parameters/containers. 
      
     @param src the pointer to the original (initialized ?) model to be 
     copied 
   */
  virtual void copyFreshModel(Model* src);

  /**
     a print function to describe a fixedInst instantiation. 
   */
  virtual void print();

  /**
     This Institution will build its facilities on the tick
     if it is the first tick of the simulation. It ignores
     all other ticks.
      
     @param time is the time to perform the tick 
   */
  virtual void handleTick(int time);

  /**
     the facilities this inst built at time = t0
   */
  std::queue<Model*> facilities() { return facilities_;}

 private:
  /**
     the list of facilities in the Fixed Inst, built at time == t0
   */
  std::queue<Model*> facilities_;

/* ------------------- */ 

};

#endif
