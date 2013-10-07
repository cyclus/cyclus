// StubMarket.h
#ifndef _STUBMARKET_H
#define _STUBMARKET_H

#include "Logger.h"
#include "MarketModel.h"
#include "QueryEngine.h"

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
class StubMarket : public MarketModel {
/* --------------------
 * all MODEL classes have these public members
 * --------------------
 */
 public:
  /**
     Default constructor for StubMarket Class 
   */
  StubMarket();

  /**
     every model should be destructable 
   */
  virtual ~StubMarket();
    
  /**
     Initializes module data members from the data contained in QueryEngine object
      
     @param qe is a pointer to a QueryEngine object containing initialization data
   */
  virtual void initModuleMembers(QueryEngine* qe);
  
  /**
     Initializes data members by cloning those of the source MarketModel
      
     @param src is the MarketModel to copy 
   */
  virtual void cloneModuleMembersFrom(MarketModel* src);

  /**
     every model should be able to print a verbose description 
   */
   virtual std::string str();

/* ------------------- */ 


/* --------------------
 * all COMMUNICATOR classes have these members
 * --------------------
 */
 public:
   /**
      The StubMarket should ignore incoming messages 
    */
   virtual void receiveMessage(msg_ptr msg);
   
/* -------------------- */


/* --------------------
 * all MARKETMODEL classes have these members
 * --------------------
 */
 public:
   /**
      Primary funcation of a Market is to resolve the set of 
      requests with the set of offers. 
       
      In this stub - do nothing! 
    */
   virtual void resolve();

/* -------------------- */


/* --------------------
 * _THIS_ MARKETMODEL class these members
 * --------------------
 */

/* -------------------- */

};

#endif
