// SourceFacility.h
#if !defined(_SOURCEFACILITY_H)
#define _SOURCEFACILITY_H
#include <iostream>
#include <deque>
#include <queue>

#include "FacilityModel.h"

/**
 * The SourceFacility class inherits from the FacilityModel class and is dynamically
 * loaded by the Model class when requested.
 * 
 * This FacilityModel will provide a simple source of some capacity (possibly infinite)
 * of some Commodity/Recipe
 *
 */
class SourceFacility : public FacilityModel  
{
/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

public:
  /**
   * Default Constructor for the SourceFacility class
   */
  SourceFacility();
  
  /**
   * Destructor for the SourceFacility class
   */
  ~SourceFacility();

  // different ways to populate an object after creation
  /// initialize an object from XML input
  virtual void init(xmlNodePtr cur);

  /// initialize an object by copying another
  virtual void copy(SourceFacility* src);

  /**
   * This drills down the dependency tree to initialize all relevant parameters/containers.
   *
   * Note that this function must be defined only in the specfici model in question and not in any 
   * inherited models preceding it.
   *
   * @param src the pointer to the original (initialized ?) model to be copied
   */
  virtual void copyFreshModel(Model* src);

  /**
   * Print information about this model
   */
  virtual void print();

/* ------------------- */ 

/* --------------------
 * all COMMUNICATOR classes have these members
 * ------------------
 */
public:
  /**
   * When this facility receives a message, execute the transaction therein.
   */
  virtual void receiveMessage(Message* msg);

/* -------------------- */

/* --------------------
 * all FACILITYMODEL classes have these members
 * --------------------
 */

public:
  /**
   * Sends material from this facility's inventory to another facility.
   *
   * @param trans is the transaction object representing the order
   * @param requester is the communicator that the transaction is sent to
   */
  virtual void sendMaterial(Transaction trans, const Communicator* requester);
  
  /**
   * Receives material sent from another facility.
   *
   * @param trans is the transaction object representing the order
   * @param manifest is the set of materials to be received.
   */
  virtual void receiveMaterial(Transaction trans, vector<Material*> manifest){};

  /**
   * Each facility is prompted to do its beginning-of-time-step
   * stuff at the tick of the timer.
   *
   * @param time is the time to perform the tick
   */
  virtual void handleTick(int time);

  /**
   * Each facility is prompted to its end-of-time-step
   * stuff on the tock of the timer.
   * 
   * @param time is the time to perform the tock
   */
  virtual void handleTock(int time);

protected: 

/* ------------------- */ 

/* --------------------
 * _THIS_ FACILITYMODEL class has these members
 * --------------------
 */

protected:
  /**
   * This facility has only one output commodity
   */
  Commodity* out_commod;
  
  /**
   * This facility has a specific recipe for its output
   */
  Material* recipe;
  
  /**
   *  The capacity is defined in terms of the number of units of the recipe
   *  that can be provided each time step.  A very large number can be
   *  provided to represent infinte capacity.
   */
  double capacity;

  /**
   * The maximum size that the inventory can grow to.
   * The NullFacility must stop processing the material in its stocks 
   * when its inventory is full.
   */
  int inventory_size;

  /**
   * The price that the facility will charge for its output commodity.
   * Units vary and are in dollars per inventory unit.
   */
  double commod_price;

	/**
	 * A collection  that holds the "product" Material this Facility has on 
	 * hand to send to others. For instance, a Reactor's inventory is its 
	 * collection of old fuel assemblies that have come out of the core.
	 */ 
	deque<Material*> inventory;
  
  /// return the inventory
  deque<Material*>* getInventory(){return &inventory;};

  /**
   * A list of orders to be processed on the Tock
   */
  deque<Message*> ordersWaiting;

  /**
   * return the total mass of the material objects in the inventory
   * the units vary and are associated with with material type
   */
  Mass checkInventory();


/* ------------------- */ 

};

/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

extern "C" Model* construct() {
  return new SourceFacility();
}

extern "C" void destruct(Model* p) {
  delete p;
}

/* ------------------- */ 

#endif
