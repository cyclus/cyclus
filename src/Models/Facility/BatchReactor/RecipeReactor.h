// BatchReactor.h
#if !defined(_BATCHREACTOR_H)
#define _BATCHREACTOR_H

#include "FacilityModel.h"

#include "Material.h"

#include "Logger.h"

#include <iostream>
#include <queue>

// Useful typedefs
typedef std::pair< std::string, mat_rsrc_ptr> Fuel; 
typedef std::pair< std::string, IsoVector> Recipe; 

/**
   Defines all possible phases this facility can be in
 */
enum Phase {BEGIN, CONTINUE, REFUEL, END};

/**
   @class BatchReactor 
   This class is identical to the RecipeReactor, except that it
   operates in a batch-like manner, i.e. it refuels in batches.
 */
class BatchReactor : public FacilityModel  {
/* --------------------
 * all MODEL classes have these members
 * --------------------
 */
 public:  
  /**
     Constructor for the BatchReactor class. 
   */
  BatchReactor();
  
  /**
     Destructor for the BatchReactor class. 
   */
  virtual ~BatchReactor() {};
    
  /**
     initialize an object from XML input 
   */
  virtual void init(xmlNodePtr cur);

  /**
     initialize an object by copying another 
   */
  virtual void copy(BatchReactor* src);

  /**
     This drills down the dependency tree to initialize all relevant 
     parameters/containers. 
      
     Note that this function must be defined only in the specific model 
     in question and not in any inherited models preceding it. 
      
     @param src the pointer to the original (initialized ?) model to be 
   */
  virtual void copyFreshModel(Model* src);

  /**
     Print information about this model 
   */
  virtual void print();

/* ------------------- */ 


/* --------------------
 * all COMMUNICATOR classes have these members
 * --------------------
 */
 public:
  /**
     When the facility receives a message, execute any transaction 
   */
  virtual void receiveMessage(msg_ptr msg);

/* ------------------- */ 


/* --------------------
 * _THIS_ MODEL class has these members
 * --------------------
 */
  /**
     Transacted resources are extracted through this method 
      
     @param order the msg/order for which resource(s) are to be prepared 
     @return list of resources to be sent for this order       
   */ 
  virtual std::vector<rsrc_ptr> removeResource(msg_ptr order);

  /**
     Transacted resources are received through this method 
      
     @param msg the transaction to which these resource objects belong 
     @param manifest is the set of resources being received 
   */ 
  virtual void addResource(msg_ptr msg,
			   std::vector<rsrc_ptr> manifest);

  /**
     The handleTick function specific to the BatchReactor. 
     At each tick, it requests as much raw inCommod as it can process 
     this month and offers as much outCommod as it will have in its 
     inventory by the 
     end of the month. 
      
     @param time the time of the tick 
   */
  virtual void handleTick(int time);

  /**
     The handleTick function specific to the BatchReactor. 
     At each tock, it processes material and handles orders, and records 
     this month's actions. 
      
     @param time the time of the tock 
   */
  virtual void handleTock(int time);

  /**
     The BatchReactor reports a power capacity of its capacity factor   
   */
  double powerCapacity(){ return CF_*capacity_;};

  /**
     return the cycle length 
   */
  int cycleLength();

  /**
     set the cycle length 
   */
  void setCycleLength(int length);

  /**
     return the capacity 
   */
  double capacity();

  /**
     set the capacity 
   */
  void setCapacity(double cap);

  /**
     return the inventory size 
   */
  double inventorySize();

  /**
     set the inventory size 
   */
  void setInventorySize(double size);

  /**
     return the facility lifetime 
   */
  int facLife();

  /**
     set the facility lifetime 
   */
  void setFacLife(int lifespan);

  /**
     return the capacity factor 
   */
  double capacityFactor();

  /**
     set the capacity factor 
   */
  void setCapacityFactor(double cf);

  /**
     return the input recipe 
   */
  IsoVector inRecipe();

  /**
     set the input recipe 
   */
  void setInRecipe(IsoVector recipe);

  /**
     set the cycle length 
   */
  IsoVector outRecipe();

  /**
     set the output recipe 
   */
  void setOutRecipe(IsoVector recipe);

  /**
     add a fuel pair 
      
     @param incommod the input commodity 
     @param inFuel the isotopics of the input fuel 
     @param outcommod the output commodity 
     @param outFuel the isotopics of the output fuel 
   */
  void addFuelPair(std::string incommod, IsoVector inFuel, 
		   std::string outcommod, IsoVector outFuel);

  /**
     return the input commodity 
   */
  std::string inCommod();

  /**
     return the output commodity 
   */
  std::string outCommod();

  /**
     get the total mass of the stuff in the inventory 
      
     @return the total mass of the processed materials in storage 
   */
  double inventoryMass();

  /**
     get the total mass of the stuff in the stocks 
      
     @return the total mass of the raw materials in storage 
   */
  double stocksMass();

 private:
  /**
     Perform the actions that must occur at the begining of the cycle 
   */
  void beginCycle();

  /**
     Perform the actions that must occur at the end of the cycle 
   */
  void endCycle();

  /**
     The receipe of input materials. 
   */
  IsoVector in_recipe_;

  /**
     The receipe of the output material. 
   */
  IsoVector out_recipe_;

  /**
     The BatchReactor has pairs of input and output fuel 
   */
  std::deque< std::pair< Recipe, Recipe > > fuelPairs_;

  /**
     Fresh fuel assemblies on hand. 
   */
  std::deque<Fuel> stocks_;

  /**
     The fuel assembly currently in the core. 
   */
  std::deque<Fuel> currCore_;
    
  /**
     Inventory of spent fuel assemblies. 
   */
  std::deque<Fuel> inventory_;

  /**
     The list of orders to process on the Tock 
   */
  std::deque<msg_ptr> ordersWaiting_;

  /**
     The time between batch reloadings. 
   */
  int cycle_length_;

  /**
     The current month in the cycle. 1 > month_in_cycle < cycle_time) 
   */
  int month_in_cycle_;

  /**
     make reqests 
   */
  void makeRequests();

  /**
     make offers 
   */
  void makeOffers();

  /**
     send messages up through the institution 
      
     @param recipient the final recipient 
     @param trans the transaction to send 
   */
  void sendMessage(Communicator* recipient, Transaction trans);

  /**
     The BatchReactor has a limit to how material it can process. 
     Units vary. It will be in the commodity unit per month. 
   */
  double capacity_;

  /**
     The maximum (number of commodity units?) that the inventory can 
     grow to. The BatchReactor must stop processing the material in its 
     stocks when its inventory is full. 
   */
  double inventory_size_;

  /**
     The number of months that a facility stays operational. 
   */
  int lifetime_;

  /**
     The year in which construction of the facility begins. 
     (maybe this should just be in the deployment description?) 
   */
  int startConstrYr_;

  /**
     The month in which construction of the facility begins. 
     (maybe this should just be in the deployment description?) 
   */
  int startConstrMo_;

  /**
     The year in which operation of the facility begins. 
     (maybe this should just be in the deployment description?) 
   */
  int startOpYr_;

  /**
     The month in which operation of the facility begins. 
     (maybe this should just be in the deployment description?) 
   */
  int startOpMo_;

  /**
     The year in which the facility's lisence expires. 
     (maybe this should just be in the deployment description?) 
   */
  int licExpYr_;

  /**
     The month in which the facility's lisence expires.
     (maybe this should just be in the deployment description?) 
   */
  int licExpMo_;

  /**
     The state in which the facility exists. 
   */
  std::string state_;

  /**
     The type of reactor that this is. 
     (this may be redundant or unnecessary information.) 
   */
  std::string typeReac_;

  /**
     The percent of the time the facility functions at 100% capacity. 
     (it should be less than one. Double check that.) 
   */
  double CF_;

/* ------------------- */ 

};

#endif

