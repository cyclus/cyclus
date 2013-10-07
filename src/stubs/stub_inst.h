// StubInst.h
#ifndef _STUBINST_H
#define _STUBINST_H

#include "Logger.h"
#include "InstModel.h"

/**
   @class StubInst 
    
   This InstModel is intended 
   as a skeleton to guide the implementation of new InstModel models. 
    
   The StubInst class inherits from the InstModel class and is 
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

class StubInst : public InstModel {
/* --------------------
 * all MODEL classes have these members
 * --------------------
 */
 public:
  /**
     Default constructor for StubInst Class 
   */
  StubInst();

  /**
     every model should be destructable 
   */
  virtual ~StubInst();
    
  /**
     Initializes the module member data from the data in a QueryEngine object
      
     @param qe is a QueryEngine object that contains intialization data
   */
  virtual void initModuleMembers(QueryEngine* qe);
  
  /**
     Copies the data from one object to another 
      
     @param src is the InstModel to copy 
   */
  virtual void cloneModuleMembersFrom(InstModel* src) ;

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
      The StubInst should ignore incoming messages 
    */
   virtual void receiveMessage(msg_ptr msg);
   
/* -------------------- */


/* --------------------
 * all INSTMODEL classes have these members
 * --------------------
 */

/* ------------------- */ 


/* --------------------
 * This INSTMODEL class has these members
 * --------------------
 */

/* ------------------- */ 

};

#endif
