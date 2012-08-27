// InstModel.h
#if !defined(_INSTMODEL_H)
#define _INSTMODEL_H

#include "TimeAgent.h"
#include "Communicator.h"
#include "Model.h"
#include "RegionModel.h"

#include <map>
#include <set>
#include <list>

class QueryEngine;
class Prototype;

// Usefull Typedefs
typedef std::set<Prototype*> PrototypeSet;
typedef std::set<Prototype*>::iterator PrototypeIterator;

/**
   The InstModel class is the abstract class/interface 
   used by all institution models 
    
   @section introduction Introduction 
   The InstModel type assists in defining the 
   region-institution-facility hierarchy in Cyclus. A InstModel 
   institution is an actor associated with a set of facilities 
   for which it is responsible. An InstModel may be used to help 
   MarketModel implementations to make material routing decisions 
   based on interfacility relationships. Deployment is a primary 
   differentiator between different InstModel implementations. 
    
   Like all model implementations, there are a number of 
   implementations that are distributed as part of the core Cyclus 
   application as well as implementations contributed by third-party 
   developers. The links below descrie parameters necessary for the 
   complete definition of an implemented instituion. 
    
   @section availableCoreImpl Available Core Implementations 
   - FixedInst: This institution is unchanging and is statically 
   associated with facilities that are deployed by some other 
   entity (such as the region or the logician. 
    
   @section anticipatedCoreImpl Anticipated Core Implementations 
   - DeploymentInst: This institution deploys allowed facilities 
   according to a demand curve. 
    
   @section thirdPartyImpl Third Party Implementations 
   (None) 
 */
class InstModel : public TimeAgent, public Communicator {
/* --------------------
 * all MODEL classes have these members
 * --------------------
 */
 public:
  /**
     Default constructor for InstModel Class 
   */
  InstModel();

  /**
     every model should be destructable 
   */
  virtual ~InstModel() {};
      
  /**
     Initalize the InstModel from a QueryEngine. Calls the init function. 
     
     @param qe a pointer to a QueryEngine object containing intialization data
   */
  virtual void initCoreMembers(QueryEngine* qe);

  /**
     every model should be able to print a verbose description 
   */
  virtual std::string str();

 public:
  /**
     default InstModel receiver is to ignore message. 
   */
  virtual void receiveMessage(msg_ptr msg);
  
  /**
     Each inst is prompted to do its beginning-of-life-step 
     stuff before the simulation begins. 
      
     Normally, inst.s simply hand the command down to facilities. 
   */
  virtual void handlePreHistory();

  /**
     Each institution is prompted to do its beginning-of-time-step 
     stuff at the tick of the timer. 
     Default behavior is to ignore the tick. 
      
     @param time is the time to perform the tick 
   */
  virtual void handleTick(int time);

  /**
     Each institution is prompted to its end-of-time-step 
     stuff on the tock of the timer. 
     Default behavior is to ignore the tock. 
      
     @param time is the time to perform the tock 
   */
  virtual void handleTock(int time);

  /**
     Each inst is prompted to do its daily tasks. 
      
     Normally, insts simply hand the command down to facilities. 
      
     @param time is the number of months since the beginning of the 
     simulation @param day is the current day in this month 
   */
  virtual void handleDailyTasks(int time, int day);

  /**
     perform all tasks required when an inst enters the simulation
   */
  virtual void enterSimulation(Model* parent);
/* ------------------- */ 


/* --------------------
 * all INSTMODEL classes have these members
 * --------------------
 */
 protected:
  /**
     The Inst's set of available prototypes to build 
   */
  PrototypeSet prototypes_;

  /**
     the initial prototypes to build
   */
  std::map<Prototype*,int> initial_build_order_;

  /**
     Adds a prototype build order to initial_build_order_
     @param qe a pointer to a QueryEngine object containing intialization data
   */
  void addPrototypeToInitialBuild(QueryEngine* qe);

 public:
  /**
     return the number of prototypes this inst can build
   */
  int nPrototypes() { return prototypes_.size(); }
  
  /**
     return the first prototype
   */
  PrototypeIterator beginPrototype() { return prototypes_.begin(); }

  /**
     return the last prototype
   */
  PrototypeIterator endPrototype() { return prototypes_.end(); }

  /**
     Checks if prototype is in the prototype list 
   */
  bool isAvailablePrototype(Prototype* p) {
    return ( prototypes_.find(p) != prototypes_.end() ); 
  }

  /**
     another moniker for isAvailablePrototype
     @param prototype the prototype to be built
   */
  virtual bool canBuild(Prototype* prototype) {
    return isAvailablePrototype(prototype);
  }
  
  /**
     checks if a prototype is in its list of available prototypes
     if not, it throws an error
     @param p the prototype to check for
   */
  void throwErrorIfPrototypeIsntAvailable(Prototype* p);

  /**
     returns this institution's region 
   */
  RegionModel* getRegion() { return (dynamic_cast<RegionModel*>( this->parent() )); };

  /**
     reports number of facilities in this inst 
   */
  int getNumFacilities(){ return this->nChildren();};

  /**
     builds a prototype 
     @param prototype the prototype to build
   */
  void build(Prototype* prototype);

/* ------------------- */ 
  
};

#endif

