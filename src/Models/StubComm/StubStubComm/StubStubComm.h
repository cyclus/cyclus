// StubStubComm.h
#if !defined(_STUBSTUBCOMM_H)
#define _STUBSTUBCOMM_H

#include "StubCommModel.h"

#include "Logger.h"

/**
   @brief 
   The StubStubComm class inherits from the StubModel class and is dynamically
   loaded by the Model class when requested.
   
   This model will do nothing. This StubCommModel is intended as a skeleton to guide
   the implementation of new StubComm models. 
 */
class StubStubComm : public StubCommModel {
/* --------------------
 * all MODEL classes have these members
 * --------------------
 */
 public:
  /**
   * @brief Default constructor for StubStub Class
   */
  StubStubComm();
  
  /**
   * @brief every model should be destructable
   */
  virtual ~StubStubComm();
    
  /**
   * @brief every model needs a method to initialize from XML
   *
   * @param cur is the pointer to the model's xml node 
   */
  virtual void init(xmlNodePtr cur);
  
  /**
   * @brief every model needs a method to copy one object to another
   *
   * @param src is the StubStub to copy
   */
  virtual void copy(StubStubComm* src) ;
  
  /**
   * @brief every model should be able to print a verbose description
   */
  virtual void print();

/* ------------------- */ 


/* --------------------
 * all COMMUNICATOR classes have these members
 * --------------------
 */
 public:
  /**
   * @brief The StubStubComm should ignore incoming messages
   */
  virtual void receiveMessage(msg_ptr msg);
  
/* -------------------- */


/* --------------------
 * all STUBCOMMMODEL classes have these members
 * --------------------
 */

/* ------------------- */ 

};


#endif
