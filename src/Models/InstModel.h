// InstModel.h
#if !defined(_INSTMODEL_H)
#define _INSTMODEL_H

#include "TimeAgent.h"
#include "Communicator.h"
#include "RegionModel.h"

/*!
@brief the InstModel class is the abstract class/interface 
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
- DeploymentInst: This institution deploys allowed facilities according to a demand curve.
*/

class InstModel : public TimeAgent, public Communicator {

/* --------------------
 * all MODEL classes have these members
 * --------------------
 */
public:
  /// Default constructor for InstModel Class
  InstModel() {
    setModelType("Inst");
  };

  /// every model should be destructable
  virtual ~InstModel() {};
  
  // every model needs a method to initialize from XML
  virtual void init(xmlNodePtr cur);

  // every model needs a method to copy one object to another
  virtual void copy(InstModel* src);

  /**
   * This drills down the dependency tree to initialize all relevant parameters/containers.
   *
   * Note that this function must be defined only in the specific model in question and not in any 
   * inherited models preceding it.
   *
   * @param src the pointer to the original (initialized ?) model to be copied
   */
  virtual void copyFreshModel(Model* src)=0;

  // every model should be able to print a verbose description
  virtual void print();

public:
  /// default InstModel receiver is to ignore message.
  virtual void receiveMessage(Message* msg);
  
  /**
   * Each inst is prompted to do its beginning-of-life-step
   * stuff before the simulation begins.
   *
   * Normally, inst.s simply hand the command down to facilities.
   *
   */
  virtual void handlePreHistory();

  /**
   * Each institution is prompted to do its beginning-of-time-step
   * stuff at the tick of the timer.
   * Default behavior is to ignore the tick.
   *
   * @param time is the time to perform the tick
   */
  virtual void handleTick(int time);

  /**
   * Each institution is prompted to its end-of-time-step
   * stuff on the tock of the timer.
   * Default behavior is to ignore the tock.
   * 
   * @param time is the time to perform the tock
   */
  virtual void handleTock(int time);

  /**
   * Each inst is prompted to do its daily tasks.
   *
   * Normally, insts simply hand the command down to facilities.
   *
   * @param time is the number of months since the beginning of the simulation
   * @param day is the current day in this month
   */
  virtual void handleDailyTasks(int time, int day);



protected:


/* ------------------- */ 
/* --------------------
 * all INSTMODEL classes have these members
 * --------------------
 */

public:
  /// returns this institution's region
  RegionModel* getRegion() { return (dynamic_cast<RegionModel*>( this->parent() )); };

  /// reports number of facilities in this inst
  int getNumFacilities(){ return this->nChildren();};

  /// queries the power capacity of each facility in the institution
  double getPowerCapacity();

  /// attempts to build another facility of type fac
  virtual bool pleaseBuild(Model* fac);

protected:
/* ------------------- */ 
  
};

#endif

