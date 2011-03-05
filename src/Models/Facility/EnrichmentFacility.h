// EnrichmentFacility.h
#if !defined(_ENRICHMENTFACILITY_H)
#define _ENRICHMENTFACILITY_H
#include <iostream>
#include <queue>

#include "FacilityModel.h"

/**
 * The EnrichmentFacility class inherits from the FacilityModel class and is dynamically
 * loaded by the Model class when requested.
 * 
 * This facility model does very little.  New material is added to queue inventory
 * and old material is removed from the same queue inventory.
 *
 */
class EnrichmentFacility : public FacilityModel  
{
/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

public:
  /** 
   * Default constructor for the EnrichmentFacility class.
   */
  EnrichmentFacility() {};

  /**
   * Destructor for the EnrichmentFacility class. 
   */
  ~EnrichmentFacility() {};
  
  // different ways to populate an object after creation
  /// initialize an object from XML input
  virtual void init(xmlNodePtr cur);

  /// initialize an object by copying another
  virtual void copy(EnrichmentFacility* src);
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
    virtual void sendMaterial(Transaction trans, const Communicator* receiver);
    
    /**
     * The facility receives the materials other facilities have sent.
     *
     * @param trans the Transaction object defining the order being filled
     * @param manifest the list of material objects being received
     */
    virtual void receiveMaterial(Transaction trans, vector<Material*> manifest);

    /**
     * The handleTick function specific to the EnrichmentFacility.
     * At each tick, it requests as much raw inCommod as it can process this
     * month and offers as much outCommod as it will have in its inventory by the
     * end of the month.
     *
     * @param time the time of the tick
     */
    virtual void handleTick(int time);

    /**
     * The handleTick function specific to the EnrichmentFacility.
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
     * A typedef for the data structure representing a Facility's process
     * line, that is, the black box materials sit in while they're being
     * operated on.
     */
    typedef multimap<int, pair<Message*, Material*> > ProcessLine;

    /**
     * The EnrichmentFacility has one input commodity
     */
    Commodity* in_commod;

    /**
     * The EnrichmentFacility has one output commodity
     */
    Commodity* out_commod;

    /**
     * The EnrichmentFacility has a limit to how material it can process.
     * Units vary. It will be in the commodity unit per month.
     */
    double capacity;

    /**
     * The EnrichmentFacility has a default tails fraction (xw)
     * to help it calculate SWUs from a desired product.
     * If it is not set in the XML input, the default value is 0.25% 
     */
    double default_xw;

    /**
     * The stocks of raw material available to be processed.
     */
    deque<Material*> stocks;
    
    /**
     * The inventory of processed material.
     */
    deque<Material*> inventory;

    /**
     * The inventory of waste material.
     */
    deque<Material*> wastes;

   	/**
     * The total mass flow required to process all outstanding orders this 
     * Facility has already committed to. Units are tons, sometimes of 
     * uranium and sometimes of certain isotopes. For Enrichment, they're 
     * tons U SWU. For Fuel Fab, they're tons U.
     */
    double outstMF;

    /**
     * The list of orders to process on the Tock
     */
    deque<Message*> ordersWaiting;

    /**
     * A map whose keys are times at which this Facility will finish 
     * executing a given order and the values are pairs comprising the orders 
     * themselves and the Materials each is to be made with.
     */
    ProcessLine ordersExecuting;

    /**
     * get the total mass of the stuff in the inventory
     *
     * @return the total mass of the materials in storage
     */
    Mass checkInventory();

    /**
     * get the total mass of the stuff in the inventory
     *
     * @return the total mass of the materials in storage
     */
    Mass checkStocks();
    
    /**
     * Enriches all material waiting in the ordersExecuting ProcessLine
     */
    void enrich();

    /**
     * Makes requests according to how much spotCapacity is available.
     */
    void makeRequests();

    /**
     * Makes offers according to how much spotCapacity is available.
     */
    void makeOffers();

    /**
	   * Calculates the separative work (in ton SWU) required to create a certain 
	   * amount of enriched uranium product subject to a given enrichment scheme.
	   * 
	   * Note: for now, this model assumes two-component enrichment (only 
	   * U-235 and U-238, no U-234 or U-236).
	   *
	   * @param massProdU the mass (in tons U) of product required
	   * @param xp the required product enrichment of U-235 (between 0 and 1)
	   * @param xf the feed enrichment of U-235 (between 0 and 1)
	   * @param xw the tails enrichment of U-235 (between 0 and 1)
	   * @return the separative work (in tons SWU)
	   */
	  static double calcSWUs(double massProdU, double xp, double xf, double xw);

	  /**
	   * Same as the four-argument version, but uses this Enrichment plant's 
	   * default value for tails enrichment.
	   * 
	   * Note: for now, this function assumes two-component enrichment (only 
	   * U-235 and U-238, no U-234 or U-236).
	   *
	   * @param massProdU the mass (in tons U) of product required
	   * @param xp the required product enrichment of U-235 (between 0 and 1)
	   * @param xf the feed enrichment of U-235 (between 0 and 1)
	   * @return the separative work (in tons SWU)
	   */
	   double calcSWUs(double massProdU, double xp, double xf);

    /**
     * Checks to see if the given candidate Material can be used for enrichment
     * of any of the ordersWaiting for Material. If so, returns 
     * an iterator pointing to that item. If not, returns an iterator just past 
	   * the last element.
	   *
	   * @param candMat the candidate material
	   * @return the iterator
	   */
	  multimap<int,Message*>::iterator checkOrdersWaiting(Material* candMat);

    /**
     * The time that the stock material spends in the facility.
     */
    int residence_time;

    /**
     * The maximum size that the inventory can grow to.
     * The EnrichmentFacility must stop processing the material in its stocks 
     * when its inventory is full.
     */
    int inventory_size;

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
    return new EnrichmentFacility();
}

extern "C" void destruct(Model* p) {
    delete p;
}

/* ------------------- */ 

#endif
