// InstModel.h
#if !defined(_INSTMODEL_H)
#define _INSTMODEL_H

#include "TimeAgent.h"
#include "Communicator.h"
#include "RegionModel.h"
#include "Model.h"

#include <set>

// Usefull Typedefs
typedef std::set<Model*> PrototypeSet;
typedef std::set<Model*>::iterator PrototypeIterator;

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
     Initalize members of InstModel and any other non-input
     related parameters
   */
  void init();

  /**
     Initalize the InstModel from xml. Calls the init function. 
     
     @param cur the current xml node pointer 
   */
  virtual void init(xmlNodePtr cur);

  /**
     every model needs a method to copy one object to another 
   */
  virtual void copy(InstModel* src);

  /**
     This drills down the dependency tree to initialize all relevant 
     parameters/containers.  
     Note that this function must be defined only in the specific model 
     in question and not in any inherited models preceding it. 
      
     @param src the pointer to the original (initialized ?) model to be 
   */
  virtual void copyFreshModel(Model* src)=0;

  /**
     every model should be able to print a verbose description 
   */
  virtual void print();

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

/* ------------------- */ 


/* --------------------
 * all INSTMODEL classes have these members
 * --------------------
 */
 protected:
  /**
     The Inst's set of available prototypes to build 
   */
  PrototypeSet* prototypes_;

   /**
     Add a prototype to the Insts list of prototypes 
   */
  void addPrototype(Model* prototype);  

 public:
  /**
     return the number of prototypes this inst can build
   */
  int nPrototypes() { return prototypes_->size(); }
  
  /**
     return the first prototype
   */
  PrototypeIterator beginPrototype() { return prototypes_->begin(); }

  /**
     return the last prototype
   */
  PrototypeIterator endPrototype() { return prototypes_->end(); }

  /**
     Checks if prototype is in the prototype list 
   */
  bool isAvailablePrototype(Model* prototype) {
    return ( prototypes_->find(prototype) 
	     != prototypes_->end() ); 
  }

  /**
     returns this institution's region 
   */
  RegionModel* getRegion() { return (dynamic_cast<RegionModel*>( this->parent() )); };

  /**
     reports number of facilities in this inst 
   */
  int getNumFacilities(){ return this->nChildren();};

  /**
     decommission one of this inst's children
   */
  void decommission(Model* child) {delete child;}

  /**
     queries the power capacity of each facility in the institution 
   */
  double powerCapacity();

  /**
     determines if a prototype can be built by this inst at the present
     time

     by default, returns false

     @param prototype the prototype to be built
   */
  virtual bool canBuild(Model* prototype) {return false;}

  /**
     builds a prototype requested by requester

     by default, an error is thrown.

     @param prototype the prototype to be built 
     @param requester the Model requesting that the prototype be built 
   */
  virtual void build(Model* prototype, Model* requester);

  /**
     builds a prototype with a specific name as requested by requester
     
     by default, it calls the simpler build function
   */
  virtual void build(Model* prototype, Model* requester, 
                     std::string name) { 
    build(prototype,requester); 
  }

/* ------------------- */ 
  
};

#endif

