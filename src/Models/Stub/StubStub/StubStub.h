// StubStub.h
#if !defined(_STUBSTUB_H)
#define _STUBSTUB_H

#include "Logger.h"
#include "StubModel.h"

/**
   @brief
   The StubStub class inherits from the StubModel class and is dynamically
   loaded by the Model class when requested.
   
   This model will do nothing. This StubStub is intended as a skeleton to guide
   the implementation of new StubStub models. 
 */

class StubStub : public StubModel {

/* --------------------
 * all MODEL classes have these members
 * --------------------
 */
public:
  /**
   * Default constructor for StubStub Class
   */
  StubStub();

  /**
   * every model should be destructable
   */
  virtual ~StubStub();
    
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
  virtual void copy(StubStub* src) ;

  /**
   * every model should be able to print a verbose description
   */
   virtual void print();

/* ------------------- */ 


/* --------------------
 * all STUBMODEL classes have these members
 * --------------------
 */

/* ------------------- */ 

};


#endif
