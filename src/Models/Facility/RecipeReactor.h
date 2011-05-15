// RecipeReactor.h
#if !defined(_NULLFACILITY_H)
#define _NULLFACILITY_H
#include <iostream>
#include <queue>

#include "FacilityModel.h"

/**
 * The RecipeReactor class inherits from the FacilityModel class and is dynamically
 * loaded by the Model class when requested.
 * 
 * This facility model does very little.  New material is added to queue inventory
 * and old material is removed from the same queue inventory.
 *
 */

typedef pair< Commodity*, Material*> InFuel; 
typedef pair< Commodity*, Material*> OutFuel; 

class RecipeReactor : public FacilityModel  
{
/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

public:
  /** 
   * Default constructor for the RecipeReactor class.
   */
  RecipeReactor() {};

  /**
   * Destructor for the RecipeReactor class. 
   */
  ~RecipeReactor() {};
  
  // different ways to populate an object after creation
  /// initialize an object from XML input
  virtual void init(xmlNodePtr cur);

  /// initialize an object by copying another
  virtual void copy(RecipeReactor* src);
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
     * @param trans the Transaction object defining the order being filled
     * @param receiver the ultimate facility to receive this transaction
     */
    virtual void sendMaterial(Message* msg, const Communicator* receiver);
    
    /**
     * The facility receives the materials other facilities have sent.
     *
     * @param trans the Transaction object defining the order being filled
     * @param manifest the list of material objects being received
     */
    virtual void receiveMaterial(Transaction trans, vector<Material*> manifest);

    /**
     * The handleTick function specific to the RecipeReactor.
     * At each tick, it requests as much raw inCommod as it can process this
     * month and offers as much outCommod as it will have in its inventory by the
     * end of the month.
     *
     * @param time the time of the tick
     */
    virtual void handleTick(int time);

    /**
     * The handleTick function specific to the RecipeReactor.
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
     * The RecipeReactor has pairs of input and output fuel
     */
    deque< pair< pair<Commodity*, Material*>, pair<Commodity*, Material*> > > fuelPairs;

    /**
     * The RecipeReactor has a limit to how material it can process.
     * Units vary. It will be in the commodity unit per month.
     */
    double capacity;

    /**
     * The stocks of fresh fuel assemblies available.
     */
    deque<InFuel> stocks;

    /**
     * The fuel assembly currently in the core.
     */
    deque<InFuel> currCore;
    
    /**
     * The inventory of spent fuel assemblies.
     */
    deque<OutFuel> inventory;

    /**
     * The list of orders to process on the Tock
     */
    deque<Message*> ordersWaiting;

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
     * The time between batch reloadings.
     */
    int cycle_time;

    /**
     * The current month in the cycle. 1 > month_in_cycle < cycle_time)
     */
    int month_in_cycle;

    /**
     * Perform the actions that must occur at the begining of the cycle
     */
    void beginCycle();

    /**
     * Perform the actions that must occur at the end of the cycle
     */
    void endCycle();

    /**
     * The time that the stock material spends in the facility.
     */
    int residence_time;

    /**
     * The maximum (number of commodity units?) that the inventory can grow to.
     * The RecipeReactor must stop processing the material in its stocks 
     * when its inventory is full.
     */
    Mass inventory_size;

    /**
     * The number of months that a facility stays operational.
     */
    int lifetime;

    /**
     * The year in which construction of the facility begins.
     * (maybe this should just be in the deployment description?)
     */
    int startConstrYr;

    /**
     * The month in which construction of the facility begins.
     * (maybe this should just be in the deployment description?)
     */
    int startConstrMo;

    /**
     * The year in which operation of the facility begins.
     * (maybe this should just be in the deployment description?)
     */
    int startOpYr;

    /**
     * The month in which operation of the facility begins.
     * (maybe this should just be in the deployment description?)
     */
    int startOpMo;

    /**
     * The year in which the facility's lisence expires.
     * (maybe this should just be in the deployment description?)
     */
    int licExpYr;

    /**
     * The month in which the facility's lisence expires..
     * (maybe this should just be in the deployment description?)
     */
    int licExpMo;

    /**
     * The state in which the facility exists.
     */
    string state;

    /**
     * The type of reactor that this is.
     * (this may be redundant or unnecessary information.)
     */
    string typeReac;

    /**
     * The percent of the time the facility functions at 100% capacity.
     * (it should be less than one. Double check that.)
     */
    double CF;

    /**
     * The receipe of input materials.
     */
    Material* in_recipe;

    /**
     * The receipe of the output material.
     */
    Material* out_recipe;

/* ------------------- */ 

};

/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

extern "C" Model* construct() {
    return new RecipeReactor();
}

extern "C" void destruct(Model* p) {
    delete p;
}

/* ------------------- */ 

#endif
