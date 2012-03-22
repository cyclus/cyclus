// StubRegion.h
#if !defined(_STUBREGION_H)
#define _STUBREGION_H

#include "RegionModel.h"

#include "Logger.h"

/**
  @class StubRegion
  
   This RegionModel is intended 
  as a skeleton to guide the implementation of new RegionModel models. 
  
  The StubRegion class inherits from the RegionModel class and is 
  dynamically loaded by the Model class when requested.

  @section intro Introduction
  Place an introduction to the model here. 

  @section modelparams Model Parameters
  Place a description of the required input parameters which define the model 
  implementation.

  @section optionalparams Optional Parameters
  Place a description of the optional input parameters to define the model 
  implementation.

  @section detailed Detailed Behavior
  Place a description of the detailed behavior of the model. Consider describing 
  the behavior at the tick and tock as well as the behavior upon sending and
  receiving materials and messages. 
 */
class StubRegion : public RegionModel {
/* --------------------
 * all MODEL classes have these members
 * --------------------
 */
 public:
  /**
   *  Default constructor for StubStub Class
   */
  StubRegion();
  
  /**
   *  every model should be destructable
   */
  virtual ~StubRegion();
    
  /**
   *  every model needs a method to initialize from XML
   *
   * @param cur is the pointer to the model's xml node 
   */
  virtual void init(xmlNodePtr cur);
  
  /**
   *  every model needs a method to copy one object to another
   *
   * @param src is the StubStub to copy
   */
  virtual void copy(StubRegion* src) ;
  
  /**
   *  This drills down the dependency tree to initialize all relevant parameters/containers.
   *
   * Note that this function must be defined only in the specific model in question and not in any 
   * inherited models preceding it.
   *
   * @param src the pointer to the original (initialized ?) model to be copied
   */
  virtual void copyFreshModel(Model* src){copy(dynamic_cast<StubRegion*>(src));}

  /**
   *  every model should be able to print a verbose description
   */
  virtual void print();
  
/* ------------------- */ 


/* --------------------
 * all COMMUNICATOR classes have these members
 * --------------------
 */
 public:   
  /**
   *  The StubRegion should ignore incoming messages
   */
  virtual void receiveMessage(msg_ptr msg);
  
/* -------------------- */


/* --------------------
 * all REGIONMODEL classes have these members
 * --------------------
 */

/* ------------------- */ 

};

#endif
