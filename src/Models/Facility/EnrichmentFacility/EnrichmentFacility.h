// EnrichmentFacility.h
#if !defined(_ENRICHMENTFACILITY_H)
#define _ENRICHMENTFACILITY_H

#include <iostream>
#include <queue>

#include "Logger.h"
#include "FacilityModel.h"
#include "Material.h"

/**
  @class EnrichmentFacility
  @brief The EnrichmentFacility enriches uranium. 
  
  This class inherits from the FacilityModel class and 
  is dynamically loaded by the Model class when requested.
  
  This facility model does very little.  New material is added to queue 
  inventory and old material is removed from the same queue inventory.
 
  @section Introduction 
  The EnrichmentFacility is a facility type in Cyclus which accepts unenriced 
  UF<sub>6</sub> (uuf6) and produces enriched UF<sub>6</sub> (euf6), and 
  depleted uranium tails (tails). An EnrichmentFacility generates 
  appropriately enriched UF<sub>6</sub> and offers that material on the 
  appropriate market. Shipments of this material are executed when the market 
  issues an order that the offer has been matched with a request.
 
  @section Model Parameters  
  EnrichmentFacility behavior is comprehensively defined by the following 
  parameters:
  - double capacity : The production capacity of the facility (SWU's).
  - double enrichment product fraction
  - double tails fraction
  - int startDate : The date on which the facility begins to operate (months).
  - int lifeTime : The length of time that the facility operates (months).
  - double inventorysize: the maximum mass quantity of material to be held in
    the inventory
  - string name : A non-generic name for this facility. 
  
  @section Optional Parameters  
  EnrichmentFacility behavior may also be specified with the following 
  optional parameters which have default values listed here.

  - capacityFactor : The ratio of actual production capacity to the rated 
  production capacity. Default is 1 (actual/rated).
  - double availFactor : The percent of time the facility operates at its 
  - capacity factor. Default is 100%.
  - double capitalCost : The cost of constructing and commissioning this 
  facility. Default is 0 ($).
  - double opCost : The annual cost of operation and maintenance of this 
  facility. Default is 0 ( $/year).
  - int constrTime : The number of months it takes to construct and commission 
  this facility. Default is 0 (months).
  - int decomTime : The number of months it takes to deconstruct and 
  decommission this facility. Default is 0 (months).
  - Inst inst : The institution responsible for this facility.  
  - double price: the offer price of the enriched UF,,6,, per kg. Default is 
  0 ($).

  @section Detailed Behavior
  The EnrichmentFacility starts operation when the simulation reaches the 
  month specified as the startDate. It immediately begins to request 
  unenriched UF<sub>6</sub> material at the rate determined by its capacity 
  and offers exactly as much material as it has in its inventory. When it 
  receives UF<sub>6</sub> that material is placed into its stocks to await 
  processing. The EnrichmentFacility then processes as much of the material 
  in its stocks as is allowed by its capacity. 
  
  When it processes some unenriched material into enriched UF,,6,, it adds the 
  amount it has produced to its inventory.  If an offer is matched with a 
  request, the EnrichmentFacility executes that order by subtracting the 
  quantity from its inventory and sending that amount to the requesting 
  facility. When the simulation time equals the startDate plus the lifeTime, 
  the facility ceases to operate.  
 */


class EnrichmentFacility : public FacilityModel  {
/* --------------------
 * all MODEL classes have these members
 * --------------------
 */
 public:
  /**
   * @brief Default constructor for the EnrichmentFacility class.
   */
  EnrichmentFacility() {};

  /**
   * @brief Destructor for the EnrichmentFacility class. 
   */
  virtual ~EnrichmentFacility() {};
  
  // different ways to populate an object after creation
  /**
   * @brief  initialize an object from XML input
   */
  virtual void init(xmlNodePtr cur);

  /**
   * @brief  initialize an object by copying another
   */
  virtual void copy(EnrichmentFacility* src);

  /**
   * @brief This drills down the dependency tree to initialize all relevant 
   * parameters/containers.
   *
   * Note that this function must be defined only in the specific model in 
   * question and not in any inherited models preceding it.
   *
   * @param src the pointer to the original (initialized ?) model to be copied
   */
  virtual void copyFreshModel(Model* src);

  /**
   * @brief Print information about this model
   */
  virtual void print();

/* ------------------- */ 


/* --------------------
 * all COMMUNICATOR classes have these members
 * --------------------
 */
 public:
  /**
   * @brief When the facility receives a message, execute any transaction therein
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
   * @brief Transacted resources are received through this method
   *
   * @param trans the transaction to which these resource objects belong
   * @param manifest is the set of resources being received
   */ 
  virtual void addResource(msg_ptr msg,
                              std::vector<rsrc_ptr> manifest);

  /**
   * @brief The handleTick function specific to the EnrichmentFacility.
   * At each tick, it requests as much raw inCommod as it can process this
   * month and offers as much outCommod as it will have in its inventory by 
   * the
   * end of the month.
   *
   * @param time the time of the tick
   */
  virtual void handleTick(int time);

  /**
   * @brief The handleTick function specific to the EnrichmentFacility.
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
   * @brief A typedef for the data structure representing a Facility's process
   * line, that is, the black box materials sit in while they're being
   * operated on.
   */
  typedef std::multimap<int, std::pair<msg_ptr, mat_rsrc_ptr> > ProcessLine;

  /**
   * @brief The EnrichmentFacility has one input commodity
   */
  std::string in_commod_;

  /**
   * @brief The EnrichmentFacility has one output commodity
   */
  std::string out_commod_;

  /**
   * @brief The EnrichmentFacility has a limit to how material it can process.
   * Units vary. It will be in the commodity unit per month.
   */
  double capacity_;

  /**
   * @brief The EnrichmentFacility has a default tails fraction (xw)
   * to help it calculate SWUs from a desired product.
   * If it is not set in the XML input, the default value is 0.25% 
   */
  double default_xw_;

  /**
   * @brief The stocks of raw material available to be processed.
   */
  std::deque<mat_rsrc_ptr> stocks_;
    
  /**
   * @brief The inventory of processed material.
   */
  std::deque<mat_rsrc_ptr> inventory_;

  /**
   * @brief The inventory of waste material.
   */
  std::deque<mat_rsrc_ptr> wastes_;

  /**
   * @brief The total mass flow required to process all outstanding orders this 
   * Facility has already committed to. Units are tons, sometimes of uranium 
   * and sometimes of certain isotopes. For Enrichment, they're tons U SWU. 
   * For Fuel Fab, they're tons U.
   */
  double outstMF_;

  /**
   * @brief The list of orders to process on the Tock
   */
  std::deque<msg_ptr> ordersWaiting_;

  /**
   * @brief A map whose keys are times at which this Facility will finish executing a 
   * given order and the values are pairs comprising the orders themselves and 
   * the Materials each is to be made with.
   */
  ProcessLine ordersExecuting_;

  /**
   * @brief get the total mass of the stuff in the inventory
   *
   * @return the total mass of the materials in storage
   */
  double checkInventory();

  /**
   * @brief get the total mass of the stuff in the inventory
   *
   * @return the total mass of the materials in storage
   */
  double checkStocks();
    
  /**
   * @brief Enriches all material waiting in the ordersExecuting ProcessLine
   */
  void enrich();

  /**
   * @brief Makes requests according to how much spotCapacity is available.
   */
  void makeRequests();

  /**
   * @brief Makes offers according to how much spotCapacity is available.
   */
  void makeOffers();

  /**
   * @brief Calculates the separative work (in ton SWU) required to create a certain 
   * amount of enriched uranium product subject to a given enrichment scheme.
   * 
   * Note: for now, this model assumes two-component enrichment (only U-235 
   * and U-238, no U-234 or U-236).
   *
   * @param massProdU the mass (in tons U) of product required
   * @param xp the required product enrichment of U-235 (between 0 and 1)
   * @param xf the feed enrichment of U-235 (between 0 and 1)
   * @param xw the tails enrichment of U-235 (between 0 and 1)
   * @return the separative work (in tons SWU)
   */
  static double calcSWUs(double massProdU, double xp, double xf, double xw);

  /**
   * @brief Same as the four-argument version, but uses this Enrichment plant's 
   * default value for tails enrichment.
   * 
   * Note: for now, this function assumes two-component enrichment (only U-235 
   * and U-238, no U-234 or U-236).
   *
   * @param massProdU the mass (in tons U) of product required
   * @param xp the required product enrichment of U-235 (between 0 and 1)
   * @param xf the feed enrichment of U-235 (between 0 and 1)
   * @return the separative work (in tons SWU)
   */
  double calcSWUs(double massProdU, double xp, double xf);

  /**
   * @brief Checks to see if the given candidate Material can be used for enrichment
   * of any of the ordersWaiting for Material. If so, returns an iterator 
   * pointing to that item. If not, returns an iterator just past the last 
   * element.
   *
   * @param candMat the candidate material
   * @return the iterator
   */
  std::multimap<int,msg_ptr>::iterator checkOrdersWaiting(mat_rsrc_ptr candMat);

  /**
   * @brief The time that the stock material spends in the facility.
   */
  int residence_time;

  /**
   * @brief The maximum size that the inventory can grow to.
   * The EnrichmentFacility must stop processing the material in its stocks 
   * when its inventory is full.
   */
  int inventory_size_;

  /**
   * @brief The receipe of input materials.
   */
  mat_rsrc_ptr in_recipe;

  /**
   * @brief The receipe of the output material.
   */
  mat_rsrc_ptr out_recipe;

/* ------------------- */ 

};

#endif

