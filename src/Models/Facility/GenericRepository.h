// GenericRepository.h
#if !defined(_GENERICREPOSITORY_H)
#define _GENERICREPOSITORY_H
#include <iostream>
#include <queue>

#include "FacilityModel.h"
#include "GenericRepository/Component.h"

/**
 * The GenericRepository class inherits from the FacilityModel class and is 
 * dynamically
 * loaded by the Model class when requested.
 * 
 * This facility model does very little.  New material is added to 
 * queue inventory
 * and old material is removed from the same queue inventory.
 *
 */

class GenericRepository : public FacilityModel  {
/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

public:
  /** Default constructor for the GenericRepository class.
   */
  GenericRepository() {};

  /**
   * Destructor for the GenericRepository class. 
   */
  ~GenericRepository() {};
  
  // different ways to populate an object after creation
  /**
   * initialize an object from XML input
   * @param name the name of the repository (i.e. Forsmark)
   * @param in_commods a vector of commodities acceptable to this model 
   * @param capacity the monthly acceptance rate
   * @param lifetime the length of time the repository shall function
   * @param area the areal extent of the repository footprint
   * @param startOpYr the year of construction starts
   * @param startOpMo the month of construction starts
   */
  virtual void init(string name, vector<Commodity*> in_commods, 
      double capacity, int lifetime, double area , int startOpYr,
      int startOpMo);

  /// initialize an object from XML input
  virtual void init(xmlNodePtr cur);

  /// initialize an object from a map of pointers
  virtual void init(map<string,void*> member_var_map);

  /// initialize an object by copying another
  virtual void copy(GenericRepository* src);

  /**
   * This drills down the dependency tree to initialize all relevant 
   * parameters/containers.
   *
   * Note that this function must be defined only in the specific 
   * model in question and not in any inherited models preceding it.
   *
   * @param src the pointer to the original model to be copied
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
   * When the facility receives a message, execute any transaction 
   * therein
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
     * @param msg the Message defining the order being filled
     * @param receiver the last facility to receive this transaction
     */
    virtual void sendMaterial(Message* msg, const Communicator* 
        receiver) {};
    
    /**
     * The facility receives the materials other facilities have sent.
     *
     * @param trans the Transaction defining the order being filled
     * @param manifest the list of material objects being received
     */
    virtual void receiveMaterial(Transaction trans, vector<Material*> 
        manifest);

    /**
     * The handleTick function specific to the GenericRepository.
     * At each tick, it requests as much raw inCommod as it can 
     * process this * month and offers as much outCommod as it 
     * will have in its inventory by the end of the month.
     *
     * @param time the time of the tick
     */
    virtual void handleTick(int time);

    /**
     * The handleTick function specific to the GenericRepository.
     * At each tock, it processes material and handles orders, and 
     * records this month's actions.
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

protected:
    /**
     * The GenericRepository has many input commodities
     */
    deque<Commodity*> in_commods_;

    /**
     * A limit to how quickly the GenericRepository can accept waste.
     * Units vary. It will be in the commodity unit per month.
     */
    double capacity_;

    /**
     * The areal extent of the repository footprint in sq. kilometers.
     */
    double area_;

    /**
     * The stocks of pre-emplacement waste materials.
     */
    deque<Material*> stocks_;

    /**
     * The inventory of emplaced materials.
     */
    deque<Material*> inventory_;

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
     * The maximum size to which the inventory may grow..
     * The GenericRepository must stop processing the material in its stocks 
     * when its inventory is full.
     */
    Mass inventory_size_;

    /**
     * The number of months that a facility stays operational.
     * hopefully, this repository is forever, but ust in case... 
     */
    int lifetime_;

    /**
     * The year in which operation of the facility begins.
     * (maybe this should just be in the deployment description?)
     */
    int start_op_yr_;

    /**
     * The month in which operation of the facility begins.
     * (maybe this should just be in the deployment description?)
     */
    int start_op_mo_;

    /**
     * The Far Field component
     */
    Component* far_field_;

    /**
     * The Buffer components
     */
    vector<Component*> buffers_;

    /**
     * The waste package component
     */
    vector<Component*> waste_packages_;

    /**
     * The waste form components
     */
    vector<Component*> waste_forms_;

    /**
     * Emplace the waste
     */
    void emplaceWaste() {};

/* ------------------- */ 

};

/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

extern "C" Model* construct() {
    return new GenericRepository();
}

extern "C" void destruct(Model* p) {
    delete p;
}

/* ------------------- */ 

#endif
