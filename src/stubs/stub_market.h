#ifndef STUB_MARKET_H_
#define STUB_MARKET_H_

#include <string>

#include "context.h"
#include "market_model.h"
#include "message.h"
#include "query_engine.h"

namespace stubs {

/**
   @class StubMarket 
    
   This MarketModel is intended 
   as a skeleton to guide the implementation of new MarketModel models. 
    
   The StubMarket class inherits from the MarketModel class and is 
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
class StubMarket : public cyclus::MarketModel {
/* --------------------
 * all MARKETMODEL classes have these members
 * --------------------
 */
 public:
  /**
     Constructor for StubMarket Class 
     @param ctx the cyclus context for access to simulation-wide parameters
   */
  StubMarket(cyclus::Context* ctx);

  /**
     every model should be destructable 
   */
  virtual ~StubMarket();
    
  /**
     Initialize members related to derived module class

     @param qe a pointer to a QueryEngine object containing initialization data
   */
  virtual void InitModuleMembers(cyclus::QueryEngine* qe);
  
  /**
     A verbose printer for the StubMarket
   */
   virtual std::string str();

  /**
     Initializes a StubMarket object by copying the members of another.
   */
   virtual cyclus::Model* Clone();

   /**
     The StubMarket should ignore incoming messages 
   */
   virtual void ReceiveMessage(cyclus::Message::Ptr msg);

   /**
      Primary funcation of a Market is to resolve the set of 
      requests with the set of offers. 
       
      In this stub - do nothing! 
    */
   virtual void Resolve();

/* -------------------- */


/* --------------------
 * _THIS_ MARKETMODEL class these members
 * --------------------
 */

/* -------------------- */

};

} // namespace stubs
  
#endif // STUB_MARKET_H_
