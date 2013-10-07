// StubRegion.h
#ifndef _STUBREGION_H
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
class StubRegion : public RegionModel {
/* --------------------
 * all MODEL classes have these members
 * --------------------
 */
 public:
  /**
     Default constructor for StubStub Class 
   */
  StubRegion();
  
  /**
     every model should be destructable 
   */
  virtual ~StubRegion();
    
  /**
     Initializes the model data members from data in the QueryEngine object
      
     @param qe a QueryEngine object containing initialization data
   */
  virtual void initModuleMembers(QueryEngine* qe);
  
  /**
     Copies data members based on those of the src RegionModel
      
     @param src is the RegionModel to copy 
   */
  virtual void cloneModuleMembersFrom(RegionModel* src) ;

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
     The StubRegion should ignore incoming messages 
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
