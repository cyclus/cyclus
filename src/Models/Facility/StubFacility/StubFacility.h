// StubFacility.h
#if !defined(_STUBFACILITY_H)
#define _STUBFACILITY_H

#include "Logger.h"
#include "FacilityModel.h"

/**
  @class StubFacility
  
   This FacilityModel is intended 
  as a skeleton to guide the implementation of new FacilityModel models. 
  
  The StubFacility class inherits from the FacilityModel class and is 
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
class StubFacility : public FacilityModel  {
/* --------------------
 * all MODEL classes have these members
 * --------------------
 */
 public:
  /**
   *  Default constructor for StubFacility Class
   */
  StubFacility();

  /**
   *  every model should be destructable
   */
  virtual ~StubFacility();
    
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
  virtual void copy(StubFacility* src) ;

  /**
   *  This drills down the dependency tree to initialize all relevant 
   * parameters/containers.
   *
   * Note that this function must be defined only in the specific model in 
   * question and not in any inherited models preceding it.
   *
   * @param src the pointer to the original (initialized ?) model to be copied
   */
  virtual void copyFreshModel(Model* src);

  /**
   *  every model should be able to print a verbose description
   */
   virtual void print();

  /**
   *  Transacted resources are extracted through this method
   * 
   * @param order the msg/order for which resource(s) are to be prepared
   * @return list of resources to be sent for this order
   *
   */ 
  virtual std::vector<rsrc_ptr> removeResource(msg_ptr order);

  /**
   *  Transacted resources are received through this method
   *
   * @param trans the transaction to which these resource objects belong
   * @param manifest is the set of resources being received
   */ 
  virtual void addResource(msg_ptr msg,
                              std::vector<rsrc_ptr> manifest);

/* ------------------- */ 


/* --------------------
 * all COMMUNICATOR classes have these members
 * --------------------
 */
 public:
  /**
   *  The StubFacility should ignore incoming messages
   */
  virtual void receiveMessage(msg_ptr msg);

/* -------------------- */


/* --------------------
 * all FACILITYMODEL classes have these members
 * --------------------
 */
 public:
  /**
   *  The handleTick function specific to the StubFacility.
   *
   * @param time the time of the tick
   */
  virtual void handleTick(int time);

  /**
   *  The handleTick function specific to the StubFacility.
   *
   * @param time the time of the tock
   */
  virtual void handleTock(int time);

/* ------------------- */ 


/* --------------------
 * _THIS_ FACILITYMODEL class has these members
 * --------------------
 */

/* ------------------- */ 

};

#endif

