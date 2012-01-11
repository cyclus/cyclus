// StorageFacility.h
#if !defined(_STORAGEFACILITY_H)
#define _STORAGEFACILITY_H
#include <iostream>
#include "Logger.h"
#include <queue>

#include "FacilityModel.h"

/**
  \class StorageFacility
  \brief This FacilityModel stores material.
  
  The StorageFacility class inherits from the FacilityModel class and is 
  dynamically loaded by the Model class when requested.

  \section intro Introduction

  Place an introduction to the model here. 

  \section modelparams Model Parameters

  Place a description of the required input parameters which define the model 
  implementation.

  \section optionalparams Optional Parameters

  Place a description of the optional input parameters to define the model 
  implementation.

  \section detailed Detailed Behavior

  Place a description of the detailed behavior of the model. Consider describing 
  the behavior at the tick and tock as well as the behavior upon sending and
  receiving materials and messages. 
 
 */
class StorageFacility : public FacilityModel  
{
/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

public:
  /** 
   * Default constructor for the StorageFacility class.
   */
  StorageFacility() {};

  /**
   * Destructor for the StorageFacility class. 
   */
  ~StorageFacility() {};
  
  // different ways to populate an object after creation
  /// initialize an object from XML input
  virtual void init(xmlNodePtr cur);

  /// initialize an object by copying another
  virtual void copy(StorageFacility* src);
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
   * Print information about this model
   */
  virtual void print();
  
  /**
   * @brief Transacted resources are extracted through this method
   * 
   * @param order the msg/order for which resource(s) are to be prepared
   * @return list of resources to be sent for this order
   *
   */ 
  virtual std::vector<Resource*> removeResource(Message* order);

  /**
   * Transacted resources are received through this method
   *
   * @param trans the transaction to which these resource objects belong
   * @param manifest is the set of resources being received
   */ 
  virtual void addResource(Transaction trans,
                              std::vector<Resource*> manifest);

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
     * The handlePreHistory function specific to the StorageFacility.
     * This function fills the StorageFacility with its initial stocks.
     *
     */
    virtual void handlePreHistory();

    /**
     * The handleTick function specific to the StorageFacility.
     * At each tick, it requests as much raw inCommod as it can process this
     * month and offers as much outCommod as it will have in its inventory by the
     * end of the month.
     *
     * @param time the time of the tick
     */
    virtual void handleTick(int time);

    /**
     * The handleTick function specific to the StorageFacility.
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
     * The StorageFacility has one commodity. It's the same in and out.
     */
    Commodity* incommod_;

    /**
     * The StorageFacility has a limit to how material it can process.
     * Units vary. It will be in the commodity unit per month.
     */
    double capacity_;

    /**
     * The stocks of entering material
     * These are not yet old enough to leave
     */
    deque<Material*> stocks_;
    
    /**
     * The inventory of material ready to exit
     */
    deque<Material*> inventory_;

    /**
     * The list of the entry times for each material
     */
    deque< pair<int, Material*> > entryTimes_;

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
     * The minimum time that the stock material spends in the facility.
     */
    int residence_time_;

    /**
     * The maximum (number of commodity units?) that the inventory can grow to.
     * The StorageFacility must stop processing the material in its stocks 
     * when its inventory is full.
     */
    Mass inventory_size_;

    /**
     * XML cursor for facility initial state information
     */
    xmlNodePtr initialStateCur_;

    /**
     * This function populated the Storage Facility with some inital stocks/inventory
     */
    void getInitialState(xmlNodePtr cur);

/* ------------------- */ 

};

#endif

