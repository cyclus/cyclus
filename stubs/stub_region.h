#ifndef STUB_REGION_H_
#define STUB_REGION_H_

#include <string>

#include "context.h"
#include "region_model.h"
#include "message.h"
#include "query_engine.h"

namespace cyclus {
namespace stubs {

/**
   @class StubRegion 
    
   This RegionModel is intended 
   as a skeleton to guide the implementation of new RegionModel models. 
    
   The StubRegion class inherits from the RegionModel class and is 
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
class StubRegion : public cyclus::RegionModel {
/* --------------------
 * all REGIONMODEL classes have these members
 * --------------------
 */
 public:
  /**
     Constructor for StubRegion Class 
     @param ctx the cyclus context for access to simulation-wide parameters
   */
  StubRegion(cyclus::Context* ctx);

  /**
     every model should be destructable 
   */
  virtual ~StubRegion();
    
  /**
     Initialize members related to derived module class

     @param qe a pointer to a QueryEngine object containing initialization data
   */
  virtual void InitModuleMembers(cyclus::QueryEngine* qe);
  
  /**
     A verbose printer for the StubRegion
   */
   virtual std::string str();

  /**
     Initializes a StubRegion object by copying the members of another.
   */
   virtual cyclus::Model* Clone();

   /**
     The StubRegion should ignore incoming messages 
   */
   virtual void ReceiveMessage(cyclus::Message::Ptr msg);
  
/* -------------------- */


/* --------------------
 * all REGIONMODEL classes have these members
 * --------------------
 */

/* ------------------- */ 

};

} // namespace stubs
} // namespace cyclus
  
#endif // STUB_REGION_H_
