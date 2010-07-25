// NullFacility.h
#if !defined(_NULLFACILITY_H)
#define _NULLFACILITY_H
#include <iostream>
#include <queue>

#include "FacilityModel.h"

/**
 * The NullFacility class inherits from the FacilityModel class and is dynamically
 * loaded by the Model class when requested.
 * 
 * This facility model does very little.  New material is added to queue inventory
 * and old material is removed from the same queue inventory.
 *
 */
class NullFacility : public FacilityModel  
{
/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

public:
  /** 
   * Default constructor for the NullFacility class.
   */
  NullFacility() {};

  /**
   * Destructor for the NullFacility class. 
   */
  ~NullFacility() {};
  
  // different ways to populate an object after creation
  /// initialize an object from XML input
  virtual void init(xmlNodePtr cur);

  /// initialize an object by copying another
  virtual void copy(NullFacility* src);

  /**
   * Print information about this model
   */
  virtual void print();

/* ------------------- */ 

/* --------------------
 * all COMMUNICATOR classes have these members
 * --------------------
 */

public:
  /**
   * A facility should not receive offers or requests directly
   * Throw an exception.
   */
    virtual void receiveOfferRequest(OfferRequest* msg);

/* -------------------- */

/* --------------------
 * all FACILITYMODEL classes have these members
 * --------------------
 */

public:
    /**
     * The handleTick function specific to the NullFacility.
     * At each tick, it requests as much raw inCommod as it can process this
     * month and offers as much outCommod as it will have in its inventory by the
     * end of the month.
     *
     * @param time the time of the tick
     */
    virtual void handleTick(int time);

    /**
     * The handleTick function specific to the NullFacility.
     * At each tock, it processes material and handles orders, and records this
     * month's actions.
     *
     * @param time the time of the tock
     */
    virtual void handleTock(int time);

    /**
     * This sends material up the Inst/Region/Logician line
     * to be passed back down to the receiver
     *
     * @param trans the Transaction object defining the order being filled
     * @param receiver the ultimate facility to receive this transaction
     */
    virtual void sendMaterial(Transaction trans, Communicator* receiver);
    
    /**
     * The facility receives the materials other facilities have sent.
     *
     * @param trans the Transaction object defining the order being filled
     * @param manifest the list of material objects being received
     */
    virtual void receiveMaterial(Transaction trans, vector<Material*> manifest);


/* ------------------- */ 

/* --------------------
 * _THIS_ FACILITYMODEL class has these members
 * --------------------
 */

protected:
    /**
     * The NullFacility has one input commodity
     */
    Commodity* in_commod;

    /**
     * The NullFacility has one output commodity
     */
    Commodity* out_commod;

    queue<Material*> active_inventory, used_inventory;

    int residence_time;
    int inventory_size;

    Material* in_recipe;
    Material* out_recipe;

/* ------------------- */ 

};

/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

extern "C" Model* construct() {
    return new NullFacility();
}

extern "C" void destruct(Model* p) {
    delete p;
}

/* ------------------- */ 

#endif
