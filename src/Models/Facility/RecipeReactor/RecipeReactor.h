// RecipeReactor.h
#if !defined(_RECIPEREACTOR_H)
#define _RECIPEREACTOR_H
#include <iostream>
#include "Logger.h"
#include <queue>

#include "FacilityModel.h"
#include "Material.h"

/**
   @class RecipeReactor
   @brief This FacilityModel represents a reactor using predefined fresh and
   spent fuel recipes.
   
   The RecipeReactor class inherits from the FacilityModel class and is 
   dynamically loaded by the Model class when requested.
   
   This facility model does very little.  New material is added to queue 
   inventory and old material is removed from the same queue inventory.
   
   @section intro Introduction 
   The RecipeReactorFacility is a facility type in *Cyclus* which consumes a 
   fixed fresh fuel recipe on a time scale governed by reactor cycle lengths 
   and batch sizes, and produces a fixed/corresponding spent fuel recipe at 
   the same frequency. A RecipeReactorFacility generates spentFuel, then 
   offers that material on the appropriate market. Shipments of this material 
   are executed when the market issues an order that the offer has been 
   matched with a request.
   
   @section modelparams Model Parameters
   RecipeReactorFacility behavior is fully defined by the following parameters:
   
   - double capacity: The power production capacity of the reactor (MW 
   electric).
   - int startDate: The date on which the facility begins to operate (months).
   - int lifeTime: The length of time that the facility operates (months).
   - int batPerCore: The number of batches per reactor core.
   - std::string outCommod: The commodity this reactor facility discharges 
   (e.g. freshUOX, freshMOX, etc.)
   - vector<Isos, NumDens> inComp : The composotion of the fixed fresh fuel 
   recipe that this reactor can accept.
   - std::string outCommod: The commodity this reactor facility discharges 
   (e.g. spentUOX, spentMOX, etc)
   - vector<Isos, NumDens> outComp: The composition of the fixed spent fuel 
   recipe that this reactor produces.
   
   @section optionalparams Optional Parameters 
   RecipeReactorFacility behavior may also be specified with the following 
   optional parameters which have default values listed here.
   
   - double capacityFactor: The ratio of actual power production to the rated 
   power production of the reactor. Default is 1 (actual/rated).
   - double availFactor: The percent of time the reactor operates at its 
   capacity factor. Default is 100%.
   - double capitalCost: The cost of commissioning this reactor. Default is 0 
   ($).
   - double opCost: The annual cost of operation and maintenance of this 
   reactor. Default is 0 ( $/year).
   - int constrTime: The number of months it takes to construct and commission 
   this reactor. Default is 0 (months).
   - int decomTime: The number of months it takes to deconstruct and 
   decommission this reactor. Default is 0 (months).
   - Inst* inst: The institution responsible for this reactor.  
   - string name: A non-generic name for this reactor. 
   
   @section detailed Detailed Behavior 
   The RecipeReactorFacility starts operation when the simulation reaches the 
   month specified as the startDate.   It immediately begins to produce 
   material at the rate defined by its capacity. Each month the 
   RecipeReactorFacility adds the amount it has produced to its inventory. It 
   then offers to the appropriate market exactly as much material as it has 
   in its inventory. If an offer is matched with a request, the 
   RecipeReactorFacility executes that order by subtracting the quantity from 
   its inventory and sending that amount to the requesting facility. When the 
   simulation time equals the startDate plus the lifeTime, the facility 
   ceases to operate.  
   
   @subsection handletick handleTick
   If the stocks are empty, the RecipeReactor asks for a batch. 
   The RecipeReactor offers anything in the inventory.
   If the reactor is at the end of a cycle, it begins the cycle by moving the
   current core batch to inventory,moving the stocks batch to the current core
   and reset month_in_cycle clock.
   
   @subsection handletock handleTock
   On the tock, the recipe reactor advances the month_in_cycle and
   sends appropriate materials of the outgoing recipe in the inventory in order
   to fill the ordersWaiting.
   
   @subsection addResource raddResource
   The RecipeReactor puts the material it receives in the stocks.
   
   @subsection removeResource removeResource
   Spent fuel of the output recipe is pulled from inventory to fulfill orders. 
   
   @subsection infinite If Infinite Capacity:
   The RecipeReactorFacility starts operation when the simulation reaches the 
   month specified as the startDate. Each month the RecipeReactorFacility 
   offers an infinite amount of material to the appropriate market. If there 
   is a request for that material, the RecipeReactorFacility executes that 
   order by sending that amount to the requesting facility. When the 
   simulation time equals the startDate plus the lifeTime, the facility 
   ceases to operate. 
   
   @section question Question:
   What is the best way to allow offers of an infinite amount of 
   material on a market?
*/

typedef pair< std::string, mat_rsrc_ptr> InFuel;
typedef pair< std::string, mat_rsrc_ptr> OutFuel; 

typedef pair< std::string, IsoVector> InRecipe;
typedef pair< std::string, IsoVector> OutRecipe; 

class RecipeReactor : public FacilityModel  {
/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

public:
  /** Default constructor for the RecipeReactor class.
   */
  RecipeReactor() {};

  /**
   * Destructor for the RecipeReactor class. 
   */
  virtual ~RecipeReactor() {};
  
  // different ways to populate an object after creation
  /// initialize an object from XML input
  virtual void init(xmlNodePtr cur);

  /// initialize an object by copying another
  virtual void copy(RecipeReactor* src);
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
    virtual void receiveMessage(msg_ptr msg);

/* -------------------- */

/* --------------------
 * all FACILITYMODEL classes have these members
 * --------------------
 */

public:
    /**
     * @brief Transacted resources are extracted through this method
     * 
     * @param order the msg/order for which resource(s) are to be prepared
     * @return list of resources to be sent for this order
     *
     */ 
    virtual std::vector<rsrc_ptr> removeResource(msg_ptr order);

    /**
     * Transacted resources are received through this method
     *
     * @param trans the transaction to which these resource objects belong
     * @param manifest is the set of resources being received
     */ 
    virtual void addResource(msg_ptr msg,
                                std::vector<rsrc_ptr> manifest);

    /**
     * The handleTick function specific to the RecipeReactor.
     * At each tick, it requests as much raw inCommod as it can process this
     * month and offers as much outCommod as it will have in its inventory by 
     * the
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

public:
    /**
     * The RecipeReactor reports a power capacity of its capacity factor * power 
     * capacity
     */
    double powerCapacity(){ return CF_*capacity_;};

protected:
    /**
     * The RecipeReactor has pairs of input and output fuel
     */
    deque< pair< pair<std::string, IsoVector>, pair<std::string, IsoVector> > > 
      fuelPairs_;

    /**
     * The RecipeReactor has a limit to how material it can process.
     * Units vary. It will be in the commodity unit per month.
     */
    double capacity_;

    /**
     * The stocks of fresh fuel assemblies available.
     */
    deque<InFuel> stocks_;

    /**
     * The fuel assembly currently in the core.
     */
    deque<InFuel> currCore_;
    
    /**
     * The inventory of spent fuel assemblies.
     */
    deque<OutFuel> inventory_;

    /**
     * The list of orders to process on the Tock
     */
    deque<msg_ptr> ordersWaiting_;

    /**
     * get the total mass of the stuff in the inventory
     *
     * @return the total mass of the processed materials in storage
     */
    double checkInventory();

    /**
     * get the total mass of the stuff in the stocks
     *
     * @return the total mass of the raw materials in storage
     */
    double checkStocks();

    /**
     * The time between batch reloadings.
     */
    int cycle_time_;

    /**
     * The current month in the cycle. 1 > month_in_cycle < cycle_time)
     */
    int month_in_cycle_;

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
    int residence_time_;

    /**
     * The maximum (number of commodity units?) that the inventory can grow to.
     * The RecipeReactor must stop processing the material in its stocks when 
     * its inventory is full.
     */
    double inventory_size_;

    /**
     * The number of months that a facility stays operational.
     */
    int lifetime_;

    /**
     * The year in which construction of the facility begins.
     * (maybe this should just be in the deployment description?)
     */
    int startConstrYr_;

    /**
     * The month in which construction of the facility begins.
     * (maybe this should just be in the deployment description?)
     */
    int startConstrMo_;

    /**
     * The year in which operation of the facility begins.
     * (maybe this should just be in the deployment description?)
     */
    int startOpYr_;

    /**
     * The month in which operation of the facility begins.
     * (maybe this should just be in the deployment description?)
     */
    int startOpMo_;

    /**
     * The year in which the facility's lisence expires.
     * (maybe this should just be in the deployment description?)
     */
    int licExpYr_;

    /**
     * The month in which the facility's lisence expires..
     * (maybe this should just be in the deployment description?)
     */
    int licExpMo_;

    /**
     * The state in which the facility exists.
     */
    string state_;

    /**
     * The type of reactor that this is.
     * (this may be redundant or unnecessary information.)
     */
    string typeReac_;

    /**
     * The percent of the time the facility functions at 100% capacity.
     * (it should be less than one. Double check that.)
     */
    double CF_;

    /**
     * The receipe of input materials.
     */
    IsoVector in_recipe_;

    /**
     * The receipe of the output material.
     */
    IsoVector out_recipe_;


/* --------------------
   output directory info
 * --------------------
 */
 public:
  /**
     The getter function for this facility model output dir
  */
  static std::string outputDir(){ 
    return FacilityModel::outputDir().append(outputDir_);}

 private:
  /**
     Every specific facility model writes to the output database
     location: FacilityModel::OutputDir_ + /this_facility's_handle
  */
  static std::string outputDir_;

  /**
   * make reqests
   */
  void makeRequests();

  /**
   * make offers
   */
  void makeOffers();

  /**
   * send messages up through the institution
   *
   * @param recipient the final recipient
   * @param trans the transaction to send
   */
  void sendMessage(Communicator* recipient, Transaction trans);

/* ------------------- */ 

};

#endif

