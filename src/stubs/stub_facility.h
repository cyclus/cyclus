// StubFacility.h
#ifndef STUB_FACILITY_H_
#define STUB_FACILITY_H_

#include "context.h"
#include "facility_model.h"
#include "logger.h"
#include "query_engine.h"

namespace stubs {

/**
   @class StubFacility 
    
   This FacilityModel is intended 
   as a skeleton to guide the implementation of new FacilityModel 
   models.  
   The StubFacility class inherits from the FacilityModel class and is 
   dynamically loaded by the Model class when requested. 
    
   @section intro Introduction 
   Place an introduction to the model here. 
    
   @section modelparams Model Parameters 
   Place a description of the required input parameters which define the 
   model implementation. 
    
   @section optionalparams Optional Parameters 
   Place a description of the optional input parameters to define the 
   model implementation. 
    
   @section detailed Detailed Behavior 
   Place a description of the detailed behavior of the model. Consider 
   describing the behavior at the tick and tock as well as the behavior 
   upon sending and receiving materials and messages. 
 */
class StubFacility : public cyclus::FacilityModel  {
 public:
  /**
     Constructor for StubFacility Class 
     @param ctx the cyclus context for access to simulation-wide parameters
   */
  StubFacility(cyclus::Context* ctx);

  /**
     every model should be destructable 
   */
  virtual ~StubFacility();
    
  /**
     Initialize members related to derived module class

     @param qe a pointer to a QueryEngine object containing initialization data
   */
  virtual void InitModuleMembers(cyclus::QueryEngine* qe);
  
  /**
     A verbose printer for the StubFacility
   */
   virtual std::string str();

  /**
     Initializes a StubFacility object by copying the members of another.
    */
   virtual cyclus::Model* Clone();

  /**
     Transacted resources are extracted through this method 
      
     @param order the msg/order for which resource(s) are to be prepared 
     @return list of resources to be sent for this order 
      
   */ 
   virtual std::vector<cyclus::Resource::Ptr>
       RemoveResource(cyclus::Transaction order);

  /**
     Transacted resources are received through this method 
      
     @param trans the transaction to which these resource objects belong 
     @param manifest is the set of resources being received 
   */ 
   virtual void AddResource(cyclus::Transaction trans,
                            std::vector<cyclus::Resource::Ptr> manifest);

   /**
     The StubFacility should ignore incoming messages 
   */
  virtual void ReceiveMessage(cyclus::Message::Ptr msg);

  /**
     The handleTick function specific to the StubFacility. 
      
     @param time the time of the tick 
   */
  virtual void HandleTick(int time);

  /**
     The handleTick function specific to the StubFacility. 
      
     @param time the time of the tock 
   */
  virtual void HandleTock(int time);

/* ------------------- */ 


/* --------------------
 * _THIS_ FACILITYMODEL class has these members
 * --------------------
 */

/* ------------------- */ 

};

} // namespace stub
  
#endif // STUB_FACILITY_H_

