// NullFacility.h
#if !defined(_NULLFACILITY_H)
#define _NULLFACILITY_H
#include <iostream>
#include "Logger.h"
#include <queue>

#include "FacilityModel.h"

/**
 The NullFacility class inherits from the FacilityModel class and is dynamically
 loaded by the Model class when requested.
 
 This facility model does very little.  New material is added to queue inventory
 and old material is removed from the same queue inventory.
  
  = Introduction =
  
  The NullFacility is a facility type in *Cyclus* capable of converting a 
  constant amount of a particular commodity type into the same amount of a 
  different commodity type.  A NullFacility generates material of the output 
  commodity type any time that it has been provided with material of the input 
  commodity type. It continuously makes requests for that commodity type and 
  offers the output commodity type when it has some in stock.
  
  = Model Parameters = 
  
  NullFacility behavior is comprehensively defined by the following parameters:
    * double capacity : The production capacity of the facility (units vary, but 
    * typically kg/month). *Question:* Do we want to allow this to be infinite?  
    * int startDate : The date on which the facility begins to operate (months).
    * int lifeTime : The length of time that the facility operates (months).
    * Commodity`*` inCommod : The type of commodity that this facility accepts.
    * Commodity`*` outCommod : The type of commodity that this facility 
    * produces.
  
  = Optional Parameters = 
  
  NullFacility behavior may also be specified with the following optional 
  parameters which have default values listed here.
  
    * double capacityFactor : The ratio of actual production capacity to the 
    * rated production capacity. Default is 1 (actual/rated).
    * double availFactor : The percent of time the facility operates at its 
    * capacity factor. Default is 100%.
    * double capitalCost : The cost of constructing and commissioning this 
    * facility. Default is 0 ($).
    * double opCost : The annual cost of operation and maintenance of this 
    * facility. Default is 0 ( $/year).
    * int constrTime : The number of months it takes to construct and commission 
    * this facility. Default is 0 (months).
    * int decomTime : The number of months it takes to deconstruct and 
    * decommission this facility. Default is 0 (months).
    * Inst`*` inst : The institution responsible for this facility.  string name 
    * : A non-generic name for this facility. 
  
  = Detailed Behavior = 
  
  The NullFacility starts operation when the simulation reaches the month 
  specified as the startDate.
  
  Each month, the NullFacility makes a request for the inCommod commodity type 
  at a rate corresponding to the NullFacility capacity less the amount it 
  currently has in its stocks (which begins the simulation empty). It also makes 
  an offer of any materials of the outCommod type in its inventory (which also 
  begins the simulation empty).  
  
  If a request is matched with an offer, the NullFacility executes that order by 
  adding the quantity to its stocks. It then proceeds to convert the material 
  into the outCommod commod type at the rate defined by its capacity, leaving 
  the isotopic composition and all other material attributes the same. At the 
  end of the month, it adds the quantity of outCommod it has produced to its 
  inventory and offers that cumulative amount to the appropriate market. 
  
  If an offer is matched with a request, the NullFacility executes that order by 
  subtracting the quantity from its stocks and sending that amount to the 
  requesting facility.  
  
  When the simulation time equals the startDate plus the lifeTime, the facility 
  ceases to operate.  

 */
class NullFacility : public FacilityModel {
/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

public:
  /** Default constructor for the NullFacility class.
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
   * When the facility receives a message, execute any transaction therein
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
     * @param order the Message object containing the order being filled
     * @param receiver the ultimate facility to receive this transaction
     */
    virtual void sendMaterial(Message* order, const Communicator* receiver);
    
    /**
     * The facility receives the materials other facilities have sent.
     *
     * @param trans the Transaction object defining the order being filled
     * @param manifest the list of material objects being received
     */
    virtual void receiveMaterial(Transaction trans, vector<Material*> manifest);

    /**
     * The handleTick function specific to the NullFacility.
     * At each tick, it requests as much raw inCommod as it can process this
     * month and offers as much outCommod as it will have in its inventory by 
     * the
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


/* ------------------- */ 

/* --------------------
 * _THIS_ FACILITYMODEL class has these members
 * --------------------
 */

protected:
    /**
     * The NullFacility has one input commodity
     */
    Commodity* in_commod_;

    /**
     * The NullFacility has one output commodity
     */
    Commodity* out_commod_;

    /**
     * The NullFacility has a limit to how material it can process.
     * Units vary. It will be in the commodity unit per month.
     */
    double capacity_;

    /**
     * The stocks of raw material available to be processed.
     */
    deque<Material*> stocks_;
    
    /**
     * The inventory of processed material.
     */
    deque<Material*> inventory_;

    /**
     * The list of orders to process on the Tock
     */
    deque<Message*> ordersWaiting_;

    /**
     * get the total mass of the stuff in the inventory
     *
     * @return the total mass of the processed materials in storage
     */
    Mass checkInventory();

    /**
     * get the total mass of the stuff in the stocks
     *
     * @return the total mass of the raw materials in storage
     */
    Mass checkStocks();


    /**
     * The time that the stock material spends in the facility.
     */
    int residence_time_;

    /**
     * The maximum (number of commodity units?) that the inventory can grow to.
     * The NullFacility must stop processing the material in its stocks when its 
     * inventory is full.
     */
    Mass inventory_size_;

    /**
     * The receipe of input materials.
     */
    Material* in_recipe_;

    /**
     * The receipe of the output material.
     */
    Material* out_recipe_;

/* ------------------- */ 

};

#endif

