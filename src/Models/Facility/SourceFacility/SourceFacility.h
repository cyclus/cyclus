// SourceFacility.h
#if !defined(_SOURCEFACILITY_H)
#define _SOURCEFACILITY_H

#include <iostream>
#include <deque>
#include <queue>

#include "Logger.h"
#include "FacilityModel.h"
#include "MatBuff.h"

/**
   @class SourceFacility 
   This FacilityModel provides a simple source of some capacity 
   (possibly infinite) of some commodity/Recipe. 
    
   The SourceFacility class inherits from the FacilityModel class and is 
   dynamically loaded by the Model class when requested. 
    
    
   @section introduction Introduction 
   The SourceFacility is a facility type in Cyclus capable of providing 
   a finite or infinite supply of a particular material to the 
   simulation.  A SourceFacility generates material of a certain 
   composition and commodity type, then offers that material on the 
   appropriate market. Shipments of this material are executed when the 
   market issues an order that the offer has been matched with a 
   request.  
   @section modelparams Model Parameters 
   SourceFacility behavior is comprehensively defined by the following 
   parameters: 
   -  double capacity: The production capacity of the facility (units 
   vary, but typically kg/month). Capacity is infinite if a negative 
   value is provided. 
   -  int startDate: The date on which the facility begins to operate 
   (months). 
   -  int lifeTime: The length of time that the facility operates 
   (months). -  std::string outCommod: the commodity that this facility 
   produces -  double inventorysize: the maximum quantity of material to 
   be held in the inventory 
   -  double commodprice: the price of the output material PER UNIT 
   -  map<Isos, NumDens> outComp 
    
   @section optionalparams Optional Parameters 
   SourceFacility behavior may also be specified with the following 
   optional parameters which have default values listed here. 
   -  double capacityFactor: The ratio of actual production capacity to 
   the rated production capacity. Default is 1 (actual/rated). 
   -  double availFactor: The percent of time the facility operates at 
   its capacity factor. Default is 100%. 
   -  double capitalCost: The cost of constructing and commissioning 
   this facility. Default is 0 ($). 
   -  double opCost: The annual cost of operation and maintenance of 
   this facility. Default is 0 ( $/year). 
   -  int constrTime: The number of months it takes to construct and 
   commission this facility. Default is 0 (months). 
   -  int decomTime: The number of months it takes to deconstruct and 
   decommission this facility. Default is 0 (months). 
   -  Inst* inst: The institution responsible for this facility. 
   -  string name: A non-generic name for this facility. 
    
   @section detailed Detailed Behavior 
   @subsection finite If Finite Capacity: 
   The SourceFacility starts operation when the simulation reaches the 
   month specified as the startDate. It immediately begins to produce 
   material at the rate defined by its capacity. Each month the 
   SourceFacility adds the amount it has produced to its inventory. It 
   then offers to the appropriate market exactly as much material as it 
   has in its inventory. If an offer is matched with a request, the 
   SourceFacility executes that order by subtracting the quantity from 
   its inventory and sending that amount to the requesting facility. 
   When the simulation time equals the startDate plus the lifeTime, the 
   facility ceases to operate.  
   @subsection infinite If Infinite Capacity: 
   The SourceFacility starts operation when the simulation reaches the 
   month specified as the startDate. Each month the SourceFacility 
   offers an infinite amount of material to the appropriate market. If 
   there is a request for that material, the SourceFacility executes 
   that order by sending that amount to the requesting facility. When 
   the simulation time equals the startDate plus the lifeTime, the 
   facility ceases to operate.  
   @subsection question Question: 
   What is the best way to allow offers of an infinite amount of 
   material on a market? 
 */
class SourceFacility : public FacilityModel  {
/* --------------------
 * all MODEL classes have these members
 * --------------------
 */
 public:
  /**
     Default Constructor for the SourceFacility class 
   */
  SourceFacility();
  
  /**
     Destructor for the SourceFacility class 
   */
  virtual ~SourceFacility();

  // different ways to populate an object after creation
  /**
     initialize an object from XML input 
   */
  virtual void init(xmlNodePtr cur);

  /**
     initialize an object by copying another 
   */
  virtual void copy(SourceFacility* src);

  /**
     This drills down the dependency tree to initialize all relevant 
     parameters/containers. 
      
     Note that this function must be defined only in the specific model 
     in question and not in any inherited models preceding it. 
      
     @param src the pointer to the original (initialized ?) model to be 
   */
  virtual void copyFreshModel(Model* src);

  /**
     Print information about this model 
   */
  virtual std::string str();

  /**
     Transacted resources are extracted through this method 
      
     @param order the msg/order for which resource(s) are to be prepared 
     @return list of resources to be sent for this order 
      
   */ 
  virtual std::vector<rsrc_ptr> removeResource(msg_ptr order);

/* ------------------- */ 


/* --------------------
 * all COMMUNICATOR classes have these members
 * ------------------
 */
 public:
  /**
     When this facility receives a message, execute the transaction 
   */
  virtual void receiveMessage(msg_ptr msg);

/* -------------------- */


/* --------------------
 * all FACILITYMODEL classes have these members
 * --------------------
 */
 public:
  /**
     Each facility is prompted to do its beginning-of-time-step 
     stuff at the tick of the timer. 
      
     @param time is the time to perform the tick 
   */
  virtual void handleTick(int time);

  /**
     Each facility is prompted to its end-of-time-step 
     stuff on the tock of the timer. 
      
     @param time is the time to perform the tock 
   */
  virtual void handleTock(int time);

  double inventory() {return inventory_.quantity();}
  void setInventory(double size) {inventory_.setCapacity(size);}

  double capacity() {return capacity_;}

/* --------------------
 * _THIS_ FACILITYMODEL class has these members
 * --------------------
 */
 protected:
  /**
     This facility has only one output commodity 
   */
  std::string out_commod_;
  
  /**
     This facility has a specific recipe for its output 
   */
  IsoVector recipe_;
  
  /**
     Name of the recipe this facility uses. 
   */
  std::string recipe_name_;

  /**
     The price that the facility will charge for its output commodity. 
     Units vary and are in dollars per inventory unit. 
   */
  double commod_price_;

  /**
     A list of orders to be processed on the Tock 
   */
  std::deque<msg_ptr> ordersWaiting_;

  /**
     generates a material at a given time 
     @param curr_time the current simulation time period 
   */
  void generateMaterial(int curr_time);
     
  /**
     builds a transaction 
   */
  Transaction buildTransaction();

  /**
     sends a transaction as an offer 
   */
  void sendOffer(Transaction trans);

  /**
     indicates the time just before the facility was built 
   */
  int prev_time_;

  /**
     The capacity is defined in terms of the number of units of the 
     recipe that can be provided each time step.  A very large number 
     can be provided to represent infinte capacity. 
   */
  double capacity_;

private:

  /**
     A collection  that holds the "product" Material this Facility has 
     on hand to send to others. 
   */ 
  MatBuff inventory_; // @MJG couldnt this be a RsrcBuff?
  
};

#endif

