// BuildInst.h
#if !defined(_BUILDINST_H)
#define _BUILDINST_H

#include <iostream>
#include <string>
#include <set>

#include "InstModel.h"

#include "Logger.h"

/**
   @class BuildInst 
    
   @section introduction Introduction 
   The BuildInst is an institution type in Cyclus which allows for 
   the building of facilities on its list of available prototypes. 
   The BuildInst does not directly determine when a prototype should 
   be build or which prototype to build; instead this decision is 
   assumed to be made by its parent. Accordingly, the build() 
   method can only be called by its parent. 
    
   @section modelParameters Model Parameters 
   - Vector <Model*> prototypes: The list of prototypes provided by 
   input which are available for this institution to build. 
   Note that the list of facilities built by this Institution is its 
   "children" container, which can be found in the Model class. 
    
   @section detailedBehavior Detailed Behavior 
   The BuildInst starts operation at the beginning of the simulation 
   and ends operation and the end of the simulation. It populates a 
   list of buildable prototypes when it is initialized and determines 
   its region as all Institution models do. When it receives a message, 
   it transmits that message immediately up to its parent or down to its 
   children. Finally, it can build any prototype in its list of avaiable 
   prototypes when prompted via its build() method; however this call 
   must be made by its parent. 
 */

class BuildInst : public InstModel  
{
/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

public:
  /**
     Default constructor for the build inst 
   */
  BuildInst() { init(); }
    
  /**
     Default destructor for the build inst 
   */
  virtual ~BuildInst() {};
  
  /**
     Initalize members of BuildInst and any other non-input
     related parameters
   */
  void init();
  
  /**
     Initalize the BuildInst from xml. Calls the init function. 
     
     @param cur the curren xml node pointer 
   */
  virtual void init(xmlNodePtr cur);
  
  /** 
     initialize an object by copying another 
   */
  virtual void copy(BuildInst* src);

  /**
     This drills down the dependency tree to initialize all relevant 
     parameters/containers. 
      
     @param src the pointer to the original (initialized ?) model to be 
     copied 
   */
  virtual void copyFreshModel(Model* src) {copy(dynamic_cast<BuildInst*>(src));}

  /**
     a print function to describe a BuildInst instantiation. 
   */
  virtual std::string str();

/* ------------------- */ 

/* --------------------
 * This INSTMODEL class has these members
 * --------------------
 */
 private:
  /**
     The total number of prototypes built by this BuildInst 
   */
  int totalBuildCount_;

  /**
     The actual guts of the build step. 
      
     @param prototype the prototype to build 
     @param name the name of that prototype 
   */
  void doBuild(Model* prototype, std::string name);
   
 public:
  /**
     determines if a prototype can be built by this inst at the present
     time

     by default, returns true if in this inst's list of prototypes

     @param prototype the prototype to be built
   */
  virtual bool canBuild(Model* prototype) 
  {return isAvailablePrototype(prototype);}

  /**
     The build function which must be called by this BuildInst's parent. 
     The BuildInst immediately builds the prototype. A random name is 
     given to the prototype and the full build function is called. 
      
     @param prototype the prototype to be built 
     @param requester the Model requesting that the prototype be built 
   */
  virtual void build(Model* prototype, Model* requester);

  /**
     The build function which must be called by this BuildInst's parent. 
     The BuildInst immediately builds the prototype. 
      
     @param prototype the prototype to be built 
     @param requester the Model requesting that the prototype be built 
     @param name the name for the prototype to be built 
   */
  virtual void build(Model* prototype, Model* requester, 
                     std::string name);

/* ------------------- */ 

};

#endif
