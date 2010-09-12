// StubRegion.h
#if !defined(_STUBREGION_H)
#define _STUBREGION_H
#include <iostream>

#include "RegionModel.h"

/**
 * The StubRegion class inherits from the RegionModel class and is dynamically
 * loaded by the Model class when requested.
 * 
 * This region will do nothing. This RegionModel is intended as a skeleton to guide
 * the implementation of new RegionModel models. 
 *
 */
class StubRegion : public RegionModel  
{
/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

public:
  /**
   * Default constructor for StubStub Class
   */
  StubRegion();

  /**
   * every model should be destructable
   */
  ~StubRegion();
    
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
  virtual void copy(StubRegion* src) ;

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
    * The StubRegion should never generate any messages
    */
    virtual void sendMessage();

    /**
     * The StubRegion should ignore incoming messages
     */
    virtual void receiveMessage(Message* msg);

protected:

/* -------------------- */

/* --------------------
 * all REGIONMODEL classes have these members
 * --------------------
 */

/* ------------------- */ 

};

/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

extern "C" Model* construct() {
    return new StubRegion();
}

extern "C" void destruct(Model* p) {
    delete p;
}

/* -------------------- */

#endif
