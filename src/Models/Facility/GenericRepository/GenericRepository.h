// GenericRepository.h
#if !defined(_GENERICREPOSITORY_H)
#define _GENERICREPOSITORY_H
#include <iostream>
#include "Logger.h"
#include <queue>

#include "FacilityModel.h"
#include "Component.h"

/**
 * type definition for waste stream objects
 */
typedef pair<Material*, std::string> WasteStream;

/**
  \class GenericRepository
  \brief This FacilityModel seeks to provide a generic disposal system model
  
  The GenericRepository class inherits from the FacilityModel class and is 
  dynamically loaded by the Model class when requested.

  \section intro Introduction 
  
  The GenericRepository is a facility type in *Cyclus* which represents a generic 
  disposal system. It continually requests wastes of all incoming commodities that
  it allows until the capacity has been reached. While there are new waste streams 
  in the stocks, they are conditioned into a waste form, packaged into a waste 
  package, and emplaced in a buffer.
  
  \section modelparams Model Parameters 
  
  GenericRepository behavior is comprehensively defined by the following 
  parameters:
   - double capacity : The production capacity of the facility (units vary, but 
   typically kg/month). *Question:* Do we want to allow this to be infinite?  
   - int startOpYear : The year in which the facility begins to operate .
   - int startOpMonth : The month in which the facility begins to operate .
   - int lifeTime : The length of time that the facility operates (months).
   - std::string inCommod : One or more types of commodity that this facility accepts.
   - Component`*` component : One or more types of component that facility contains
   - Inst`*` inst : The institution responsible for this facility.  
   - double area : The square  meters of topographic area that the repository may occupy.
  
   \section optionalparams Optional Parameters  
  
   GenericRepository behavior may also be specified with the following optional 
   parameters which have default values listed here...  
  
   \section detailed Detailed Behavior 
  
   The GenericRepository is under development at this time. 
  
   In general, it starts operation when the simulation reaches the month specified 
   as the startDate. Each month, the GenericRepository makes a request for the 
   inCommod commodity types at a rate corresponding to the calculated capacity less 
   the amount it currently has in its stocks (which begins the simulation empty). 
  
   If a request is matched with an offer, the GenericRepository receives that 
   order from the supplier and adds the quantity to its stocks. 
  
   At the end of the month, it then proceeds to condition, package, and load the 
   material into the repository according to thermal and nuclide constraints. 
  
   During each month, nuclide and heat transport calculations are completed within 
   the repository in order to determine useful fuel cycle metrics.
  
  When the simulation time equals the start date plus the lifetime, the facility 
  ceases to operate. 

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
    deque<std::string> in_commods_;

    /**
     * A limit to how quickly the GenericRepository can accept waste.
     * Units vary. It will be in the commodity unit per month.
     */
    double capacity_;

    /**
     * The x, y, and z dimensional extents of the repository in kilometers.
     */
    double x_,y_,z_;

    /**
     * The x, y, and z dimensional spacing of the source centers in kilometers.
     */
    double dx_,dy_,dz_;

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
    double inventory_size_;

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
     * The waste package components current being dealt with
     */
    deque<Component*> current_waste_packages_;

    /**
     * The waste package components that have been emplaced
     */
    deque<Component*> emplaced_waste_packages_;

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
     * Each commodity is associated with a waste form.
     */
    map<std::string, Component*> commod_wf_map_;

    /**
     * Each waste form associated with a waste package.
     */
    map<string, Component*> wf_wp_map_;

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
     * Make requests based on capacity
     *
     * @param time the time at which to make the requests. 
     */
    void makeRequests(int time);

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
     * Set the placement for the component within the repository
     *
     * @param comp the component to be placed
     * @return comp the component once it has been placed
     */
    Component* setPlacement(Component* comp);

    /**
     * Initializes the name and model type of the component
     *
     * @param cur the current xml pointer
     *
     * @return the initialized component
     */
    Component* initComponent(xmlNodePtr cur) ;

    /**
     * Do heat transport calculations. 
     */
    void transportHeat() ;

    /**
     * Do nuclide transport calculations
     */
    void transportNuclides() ;

    /**
     * get the commodity-specific capacity of the GenericRepository
     *
     * @param commod the commodity
     */
    double getCapacity(std::string commod) ;


/* ------------------- */ 

};

#endif

