// StubMarket.h
#if !defined(_STUBMARKET_H)
#define _STUBMARKET_H
#include <iostream>

#include "MarketModel.h"

/**
 * The StubMarket class inherits from the MarketModel class and is dynamically
 * loaded by the Model class when requested.
 * 
 * This market will do nothing. This MarketModel is intended as a skeleton to
 * guide the implementation of new MarketModel models.
 *
 */
class StubMarket : public MarketModel  
{
/* --------------------
 * all MODEL classes have these public members
 * --------------------
 */

public:
  /**
   * Default constructor for StubMarket Class
   */
  StubMarket();

  /**
   * every model should be destructable
   */
  ~StubMarket();
    
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
  virtual void copy(StubMarket* src) ;

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
    * The StubMarket should never generate any messages
    */
    virtual void sendMessage();

    /**
     * The StubMarket should ignore incoming messages
     */
    virtual void receiveMessage(Message* msg);

protected:

/* -------------------- */

/* --------------------
 * all MARKETMODEL classes have these members
 * --------------------
 */

  /// Resolve requests with offers
  /**
   *  Primary funcation of a Market is to resolve the set of 
   *  requests with the set of offers.
   *
   *  In this stub - do nothing!
   */
  virtual void resolve();


/* -------------------- */

/* --------------------
 * _THIS_ MARKETMODEL class these members
 * --------------------
 */

/* -------------------- */

};

/* --------------------
 * all MODEL classes have these members
 * --------------------
 */
extern "C" Model* construct() {
  return new StubMarket();
}

extern "C" void destruct(Model* p) {
  delete p;
}

/* -------------------- */

#endif
