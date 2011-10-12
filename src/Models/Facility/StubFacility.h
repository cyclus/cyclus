// StubFacility.h
#if !defined(_STUBFACILITY_H)
#define _STUBFACILITY_H
#include <iostream>

#include "FacilityModel.h"

/**
 * The StubFacility class inherits from the FacilityModel class and is dynamically
 * loaded by the Model class when requested.
 * 
 * This facility will do nothing. This FacilityModel is intended as a skeleton to guide
 * the implementation of new FacilityModel models. 
 *
 */
class StubFacility : public FacilityModel  
{
/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

public:
  /**
   * Default constructor for StubFacility Class
   */
  StubFacility();

  /**
   * every model should be destructable
   */
  ~StubFacility();
    
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
  virtual void copy(StubFacility* src) ;

  /**
   * This drills down the dependency tree to initialize all relevant parameters/containers.
   *
   * Note that this function must be defined only in the specific model in question and not in any 
   * inherited models preceding it.
   *
   * @param src the pointer to the original (initialized ?) model to be copied
   */
  virtual void copyFreshModel(Model* src);

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
     * The StubFacility should ignore incoming messages
     */
    virtual void receiveMessage(Message* msg);

/* -------------------- */

/* --------------------
 * all FACILITYMODEL classes have these members
 * --------------------
 */

public:
    /**
     * This sends material up the Inst/Region/Logician line
     * to be passed back down to the receiver
     *
     * @param trans the Transaction object defining the order being filled
     * @param receiver the ultimate facility to receive this transaction
     */
    virtual void sendMaterial(Transaction trans, const Communicator* receiver);
    
    /**
     * The facility receives the materials other facilities have sent.
     *
     * @param trans the Transaction object defining the order being filled
     * @param manifest the list of material objects being received
     */
    virtual void receiveMaterial(Transaction trans, vector<Material*> manifest);

    /**
     * The handleTick function specific to the StubFacility.
     *
     * @param time the time of the tick
     */
    virtual void handleTick(int time);

    /**
     * The handleTick function specific to the StubFacility.
     *
     * @param time the time of the tock
     */
    virtual void handleTock(int time);

protected:

/* ------------------- */ 

/* --------------------
 * _THIS_ FACILITYMODEL class has these members
 * --------------------
 */

/* ------------------- */ 

};

/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

extern "C" Model* construct() {
  return new StubFacility();
}

extern "C" void destruct(Model* p) {
  delete p;
}

/* ------------------- */ 

#endif
