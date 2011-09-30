// SeparationsMatrixFacility.h
#if !defined(_SEPARATIONSFACILITY_H)
#define _SEPARATIONSFACILITY_H
#include <iostream>
#include <queue>
#include <vector>

#include "FacilityModel.h"

/**
 * The SeparationsMatrixFacility class inherits from the FacilityModel class and is dynamically
 * loaded by the Model class when requested.
 * 
 * This facility will do nothing. This FacilityModel is still under construction and just
 * in testing phase now. 
 *
 */

typedef pair< Commodity*, Material*> InSep; 
typedef pair< Commodity*, Material*> OutSep;

// typedef pair< Commodity*, pair< Commodity*, Material*>  > stream_set;

class SeparationsMatrixFacility : public FacilityModel  
{
/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

public:
  /**
   * Default constructor for SeparationsMatrixFacility Class
   */
  SeparationsMatrixFacility(){};

  /**
   * every model should be destructable
   */
  ~SeparationsMatrixFacility() {};
    
  /**
   * every model needs a method to initialize from XML
   *
   * @param cur is the pointer to the model's xml node 
   */
  virtual void init(xmlNodePtr cur);
  
  /**
   * every model needs a method to initialize from a map of pointers
   *
   * @param cur is a map of pointers to member variable values
   */
  virtual void init(map<string, void*> member_var_map);
  
  /**
   * every model needs a method to copy one object to another
   *
   * @param src is the StubStub to copy
   */
  virtual void copy(SeparationsMatrixFacility* src) ;

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
   * every model should be able to print a verbose description
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
     * @param msg the message defining the order being filled
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
     * The handleTick function specific to the SeparationsMatrixFacility.
     * At each tick, it requests as much raw inCommod as it can process this
     * month and offers as much outCommod as it will have in its inventory by the
     * end of the month.
     *
     * @param time the time of the tick
     */
    virtual void handleTick(int time);

		/**
     * The handleTick function specific to the SeparationsMatrixFacility.
     * At each tock, it processes material and handles orders, and records this
     * month's actions.
     *
     * @param time the time of the tock
     */
    virtual void handleTock(int time);

protected:

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
     *  Vector of incoming material
     */
    vector<Commodity*> in_commod_;

    /**
     *  Vector of outgoing material
     */
    vector<Commodity*> out_commod_;

    /**
     * The SeparationsMatrixFacility has a limit to how material it can process.
     * Units vary. It will be in the commodity unit per month.
     */
    double capacity_;

    /**
     * This is the structure for each stream.
     */
    map<Commodity*,pair<int, double> > stream_set_;

    /**
     * The stocks of raw material available to be processed.
     */
    deque<pair<Commodity*,Material*> > stocks_;
    
    /**
     * The inventory of processed material.
     */
    deque<pair<Commodity*,Material*> > inventory_;

    /**
     * The inventory of waste material.
     */
    deque<pair<Commodity*,Material*> > wastes_;

   	/**
     * The total mass flow required to process all outstanding orders this 
     * Facility has already committed to. Units are tons, sometimes of 
     * uranium and sometimes of certain isotopes. For Enrichment, they're 
     * tons U SWU. For Fuel Fab, they're tons U.
     */
    double outstMF_;

    /**
     * The list of orders to process on the Tock
     */
    deque<Message*> ordersWaiting_;

    /**
     * A map whose keys are times at which this Facility will finish 
     * executing a given order and the values are pairs comprising the orders 
     * themselves and the Materials each is to be made with.
     */
    ProcessLine ordersExecuting_;

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
     * Separates all material waiting in the ordersExecuting ProcessLine
     */
    void separate();

    /**
     * Makes requests according to how much spotCapacity is available.
     */
    void makeRequests();

    /**
     * Makes offers according to how much spotCapacity is available.
     */
    void makeOffers();

		/**
     * Checks to see if the given candidate Material can be used for separations
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
    int residence_time_;

    /**
     * The maximum size that the inventory can grow to.
     * The SeparationsMatrixFacility must stop processing the material in its stocks 
     * when its inventory is full.
     */
    int inventory_size_;

    /**
     * Hack Force to Get Streams to operate by telling code how many streams to separate
     */
    int nodeForce_;

    /**
     * Stream of material in a list to use as Separations Guide
     * Contains matched set of element (integer) and sep fraction (double)
     */
    //struct Stream;

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

/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

extern "C" Model* construct() {
  return new SeparationsMatrixFacility();
}

extern "C" void destruct(Model* p) {
  delete p;
}

/* ------------------- */ 

#endif
