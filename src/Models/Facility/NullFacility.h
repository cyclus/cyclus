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
   * Should delete all instances of NullFacility and recursively
   * delete all of their data.
   */
  ~NullFacility() {};
  
  // different ways to populate an object after creation
  /// initialize an object from XML input
  virtual void init(xmlNodePtr cur);

  /// initialize an object by copying another
  virtual void copy(NullFacility* src);

  virtual void print();

/* ------------------- */ 

/* --------------------
 * all COMMUNICATOR classes have these members
 * --------------------
 */
public:
    /// simply ignore incoming offers/requests
    virtual void receiveOfferRequest(OfferRequest* msg);

/* -------------------- */

/* --------------------
 * all FACILITYMODEL classes have these members
 * --------------------
 */

public:
    /**
     * The NullFacility overrides the FacilityModel handleTick function.
     * At each tick, it requests as much raw inCommod as it can process this
     * month, offers as much outCommod as it will have in its inventory by the
     * end of the month.
     *
     * @param time the time of the tick
     */
    virtual void handleTick(int time);

    /**
     * The NullFacility overrides the FacilityModel handleTick function.
     * At each tick, it requests as much raw inCommod as it can process this
     * month, offers as much outCommod as it will have in its inventory by the
     * end of the month.
     *
     * @param time the time of the tick
     */
    virtual void handleTick(int time);

    /// simply do nothing when sending a shipment
    virtual void sendMaterial(Transaction trans, Communicator* receiver);
    
    /// simply do nothing when receiving a shipment
    virtual void receiveMaterial(Transaction trans, vector<Material*> manifest);


/* ------------------- */ 

/* --------------------
 * _THIS_ FACILITYMODEL class has these members
 * --------------------
 */

protected:
    /// all facilities must have at least one input commodity
    Commodity* in_commod;

    /// all facilities must have at least one output commodity
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
