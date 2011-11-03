// StubStubComm.h
#if !defined(_STUBSTUBCOMM_H)
#define _STUBSTUBCOMM_H
#include <iostream>
#include "Logger.h"

#include "StubCommModel.h"
/**
 * The StubStubComm class inherits from the StubModel class and is dynamically
 * loaded by the Model class when requested.
 * 
 * This model will do nothing. This StubCommModel is intended as a skeleton to guide
 * the implementation of new StubComm models. 
 *
 */
class StubStubComm : public StubCommModel  
{
/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

public:
  /**
   * Default constructor for StubStub Class
   */
  StubStubComm();

  /**
   * every model should be destructable
   */
  ~StubStubComm();
    
  /**
   * every model needs a method to initialize from XML
   *
   * @param cur is the pointer to the model's xml node 
   */
  virtual void init(xmlNodePtr cur);
  
  /**
   * every model needs a method to copy one object to another
   *
   * @param src is the StubStub to copy
   */
  virtual void copy(StubStubComm* src) ;

  /**
   * every model should be able to print a verbose description
   */
   virtual void print();

/* ------------------- */ 

/* --------------------
 * all COMMUNICATOR classes have these members
 * --------------------
 */
public:

    /**
     * The StubStubComm should ignore incoming messages
     */
    virtual void receiveMessage(Message* msg);

protected:

/* -------------------- */

/* --------------------
 * all STUBCOMMMODEL classes have these members
 * --------------------
 */

/* ------------------- */ 

};

/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

extern "C" Model* construct() {
    return new StubStubComm();
}

extern "C" void destruct(Model* p) {
    delete p;
}

/* -------------------- */

#endif
