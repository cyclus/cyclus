// SinkFacility.h
#if !defined(_SINKFACILITY_H)
#define _SINKFACILITY_H
#include <iostream>
#include <deque>
#include <queue>

#include "FacilityModel.h"

/**
 * The SinkFacility class inherits from the FacilityModel class and is dynamically
 * loaded by the Model class when requested.
 * 
 * This facility accepts a finite amount of its input commodity. 
 */
class SinkFacility : public FacilityModel  
{
/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

public:
  /**
   * Constructor for the SinkFacility class.
   */
  SinkFacility();
  
  /**
   * Destructor for the SinkFacility class.
   */
  ~SinkFacility();

  // different ways to populate an object after creation
  /// initialize an object from XML input
  virtual void init(xmlNodePtr cur);

  /// initialize an object by copying another
  virtual void copy(SinkFacility* src);

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
   * A verbose printer for the Sink Facility.
   */
  virtual void print();

/* ------------------- */ 

/* --------------------
 * all COMMUNICATOR classes have these members
 * --------------------
 */
public:
  /// The sink Facility doesn't need to do anything if it gets a message.
  /// It never sends any matieral to anyone.
  virtual void receiveMessage(Message* msg) {};

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
   * @param receiver is the communicator that the transaction is sent to next
   */
  virtual void sendMaterial(Transaction trans, Communicator* receiver){};
  
  /**
   * Receives material sent from another facility.
   *
   * @param trans is the transaction object representing the order
   * @param manifest is the set of materials to be received.
   */
  virtual void receiveMaterial(Transaction trans, vector<Material*> manifest);

  /**
   * The SinkFacility can handle the Tick.
   *
   * @param time the current simulation time.
   */
  virtual void handleTick(int time);

  /**
   * The SinkFacility can handle the Tock.
   *
   * @param time the current simulation time.
   */
  virtual void handleTock(int time);

/* ------------------- */ 

/* --------------------
 * _THIS_ FACILITYMODEL class has these members
 * --------------------
 */

protected:
  /// all facilities must have at least one input commodity
  vector<Commodity*> in_commods;

  /// this facility holds material in storage. 
  deque<Material*> inventory;

  /**
   * get the total mass of the stuff in the inventory
   *
   * @return the total mass of the materials in storage
   */
  Mass checkInventory();

  /// maximum inventory size
  Mass inventory_size;

  /// monthly acceptance capacity
  Mass capacity;

  /// commodity price
  double commod_price;


/* ------------------- */ 

};

/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

extern "C" Model* construct() {
  return new SinkFacility();
}

extern "C" void destruct(Model* p) {
  delete p;
}

/* ------------------- */ 

#endif
