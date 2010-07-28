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
  
  StubFacility() {};
  
  ~StubFacility() {};

  // different ways to populate an object after creation
  /// initialize an object from XML input
  virtual void init(xmlNodePtr cur);

  /// initialize an object by copying another
  virtual void copy(StubFacility* src);

  virtual void print();

/* ------------------- */ 

/* --------------------
 * all COMMUNICATOR classes have these members
 * --------------------
 */
public:
  /// simply ignore incoming offers/requests
  virtual void receiveMessage(Message* msg) {};

/* -------------------- */

/* --------------------
 * all FACILITYMODEL classes have these members
 * --------------------
 */

public:
  /// simply do nothing when sending a shipment
  virtual void sendMaterial(Transaction trans, Communicator* receiver) {};
  
  /// simply do nothing when receiving a shipment
  virtual void receiveMaterial(Transaction trans, vector<Material*> manifest) {};

  /// simply do nothing on the tick
  virtual void handleTick(int time){};

  /// simply do nothing on the tock
  virtual void handleTock(int time){};

/* ------------------- */ 

/* --------------------
 * _THIS_ FACILITYMODEL class has these members
 * --------------------
 */

protected:
  /// all facilities must have at least one input commodity
  vector<Commodity*> in_commods;

  /// all facilities must have at least one output commodity
  vector<Commodity*> out_commods;

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
