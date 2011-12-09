// SinkFacility.h
#if !defined(_SINKFACILITY_H)
#define _SINKFACILITY_H
#include <iostream>
#include "Logger.h"
#include <deque>
#include <queue>
#include <string>

#include "FacilityModel.h"

/**
  \class SinkFacility
  \brief  This FacilityModel requests a finite amount of its input commodity. It 
  offers nothing. 
  
  The SinkFacility class inherits from the FacilityModel class and is 
  dynamically loaded by the Model class when requested.
  
  \section intro Introduction 
  
  The SinkFacility is a facility type in *Cyclus* capable of accepting a finite 
  or infinite quantity of some commodity produced in the simulation.  A 
  SinkFacility requests an amount of that commodity from the appropriate market. 
  It then receives that commodity when the market issues an order that the 
  request has been matched with a corresponding offer.
  
  \section modelparams Model Parameters 
  
  SinkFacility behavior is comprehensively defined by the following parameters:
    - double capacity : The acceptance capacity of the facility (units vary, but 
    typically kg/month). Capacity is infinite if a positive value is provided.
    - int startDate : The date on which the facility begins to operate (months).
    - int lifeTime : The length of time that the facility operates (months).
    - commodity`-` inCommod : The commodity type this facility accepts.
  
  \section optionalparams Optional Parameters 
  
  SinkFacility behavior may also be specified with the following optional 
  parameters which have default values listed here.
  
    - double capacityFactor : The ratio of actual acceptance capacity to the 
    rated acceptance capacity. Default is 1 (actual/rated).
    - double availFactor : The percent of time the facility operates at its 
    capacity factor. Default is 100%.
    - double capitalCost : The cost of constructing and commissioning this 
    facility. Default is 0 ($).
    - double opCost : The annual cost of operation and maintenance of this 
    facility. Default is 0 ($/year).
    - int constrTime : The number of months it takes to construct and commission 
    this facility. Default is 0 (months).
    - int decomTime : The number of months it takes to deconstruct and 
    decommission this facility. Default is 0 (months).
    - Inst`-` inst : The institution responsible for this facility.  
    - string name : A non-generic name for this facility. 
  
  \section detailed Detailed Behavior 
  
  \subsection finite If Finite Capacity:

  The SinkFacility starts operation when the simulation reaches the month 
  specified as the startDate. It immediately begins to request the inCommod 
  commodity type at the rate defined by the SinkFacility capacity. If a request 
  is matched with an offer from another facility, the SinkFacility executes that 
  order by adding that quantity to its stocks. When the simulation time equals 
  the startDate plus the lifeTime, the facility ceases to operate.  

  \subsection infinite If Infinite Capacity:

  The SinkFacility starts operation when the simulation reaches the month 
  specified as the startDate. Each month the SinkFacility requests an infinite 
  amount of the inCommod commodity from the appropriate market. If there is a 
  corresponding offer for that commodity type from another facility, the 
  SinkFacility executes that order by adding that quantity to its stocks. When 
  the simulation time equals the startDate plus the lifeTime, the facility 
  ceases to operate.

  \subsection question Question: 

  What is the best way to allow requests of an infinite amount of 
  material on a market? 
  
 */
class SinkFacility : public FacilityModel  {
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
   * This drills down the dependency tree to initialize all relevant 
   * parameters/containers.
   *
   * Note that this function must be defined only in the specific model in 
   * question and not in any inherited models preceding it.
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
  vector<std::string> in_commods_;

  /// this facility holds material in storage. 
  deque<Material*> inventory_;

  /**
   * get the total mass of the stuff in the inventory
   *
   * @return the total mass of the materials in storage
   */
  Mass checkInventory();

  /// maximum inventory size
  Mass inventory_size_;

  /// monthly acceptance capacity
  Mass capacity_;

  /// commodity price
  double commod_price_;


/* ------------------- */ 

};

#endif

