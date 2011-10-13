// GenericRepository.h
#if !defined(_GENERICREPOSITORY_H)
#define _GENERICREPOSITORY_H
#include <iostream>
#include <queue>

#include "FacilityModel.h"
#include "GenericRepository/Component.h"

/**
 * type definition for waste stream objects
 */
typedef pair<Material*, Commodity*> WasteStream;

/**
 * @brief This model seeks to provide a generic disposal system model
 * 
 * The GenericRepository class inherits from the FacilityModel class and is 
 * dynamically loaded by the Model class when requested.
 * 
 */

class GenericRepository : public FacilityModel  {
/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

public:
  /// Default constructor for the GenericRepository class.
  GenericRepository() {};

  /// Destructor for the GenericRepository class. 
  ~GenericRepository() {};
  
  /// initialize an object from XML input
  virtual void init(xmlNodePtr cur);

  /// initialize an object by copying another
  virtual void copy(GenericRepository* src);

  /**
   * @brief deep copy method 
   *
   * This drills down the dependency tree to initialize all relevant 
   * parameters/containers.
   *
   * Note that this function must be defined only in the specific 
   * model in question and not in any inherited models preceding it.
   *
   * @param src the pointer to the original model to be copied
   */
  virtual void copyFreshModel(Model* src);

  /// Print information about this model
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
   *
   * @param msg the message to receive
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
    deque<WasteStream> stocks_;

    /**
     * The inventory of emplaced materials.
     */
    deque<WasteStream> inventory_;

    /**
     * The maximum size to which the inventory may grow..
     * The GenericRepository must stop processing the material in its stocks 
     * when its inventory is full. (YMRLegislative = 70,000tHM) 
     */
    Mass inventory_size_;

    /**
     * The number of months that a facility stays operational.
     * hopefully, this repository is forever, but just in case... 
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
     * Reports true if the repository has reached capacity, false otherwiset
     */
    bool is_full_;

    /**
     * The Environment component
     */
    Component* env_;

    /**
     * The Far Field component
     */
    Component* far_field_;

    /**
     * The Near Field component
     */
    Component* near_field_;

    /**
     * The buffer template before initialization.
     * This will be copied and initialized before use.
     */
    Component* buffer_template_;

    /**
     * The waste package component templates before initialization.
     * These will be copied and initialized before use.
     */
    deque<Component*> wp_templates_;

    /**
     * The waste form templates before initialization.
     * These will be copied and initialized before use.
     */
    deque<Component*> wf_templates_;

    /**
     * The waste package component
     */
    deque<Component*> current_waste_packages_;

    /**
     * The waste form components
     */
    deque<Component*> current_waste_forms_;

    /**
     * The buffer components
     */
    deque<Component*> buffers_;

    /**
     * The waste package component
     */
    deque<Component*> waste_packages_;

    /**
     * The waste form components
     */
    deque<Component*> waste_forms_;

    /**
     * Each Commodity is associated with a waste form.
     */
    map<Commodity*, Component*> commod_wf_map_;

    /**
     * Each waste form associated with a waste package.
     */
    map<string, Component*> wf_wp_map_;

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
     * Emplace the waste
     */
    void emplaceWaste() ;
    
    /**
     * Condition the waste
     *
     * @param waste_stream is the WasteStream object to be conditioned
     * @return the form that has been loaded with the waste stream
     */
    Component* conditionWaste(WasteStream waste_stream) ;

    /**
     * Package the waste
     *
     * @param waste_form is the loaded waste form to be packaged
     * @return the package that has been loaded with the waste form
     */
    Component* packageWaste(Component* waste_form) ;

    /**
     * Load the buffer with the waste
     *
     * @param waste_package is the package to load into the buffer
     * @return the buffer that has been loaded with the waste package
     */
    Component* loadBuffer(Component* waste_package) ;

    /**
     * Initializes the name and model type of the component
     *
     * @param cur the current xml pointer
     *
     * @return the initialized component
     */
    static Component* initComponent(xmlNodePtr cur) ;

    /**
     * Do heat transport calculations
     */
    static void transportHeat() ;

    /**
     * Do nuclide transport calculations
     */
    static void transportNuclides() ;

    /**
     * Return the component model named by the string
     *
     * @param model_name is the name of the component model (e.g. StubComponent)
     */
    static Component* getComponent(string model_name) ;

    /**
     * Return the component model enumerated type named by the string
     *
     * @param model_name is the name of the component model (e.g. StubComponent)
     */
    static RepoComponent getRepoComponentType(string model_name) ;


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
