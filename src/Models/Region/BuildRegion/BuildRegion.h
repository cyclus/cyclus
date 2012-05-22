// BuildRegion.h
#if !defined(_BUILDREGION_H)
#define _BUILDREGION_H

#include "RegionModel.h"
#include "Model.h"

#include <set>
#include <list>
#include <map>
#include <utility>

// Useful Typedefs
// Building Prototypes
typedef std::pair<Model*,int> PrototypeDemand;
typedef std::pair<int,PrototypeDemand> PrototypeBuildOrder;
typedef std::list<PrototypeBuildOrder*> PrototypeOrders;
typedef std::list<PrototypeBuildOrder*>::iterator OrderIterator;
// Sorting Model Lists
typedef std::list<Model*>::iterator ModelIterator;

/**
   The BuildRegion class inherits from the RegionModel class and is 
   dynamically loaded by the Model class when requested. 
    
   This region determines if there is a need to meet a certain capacity 
   (as defined via input) at each time step. If there is such a need, 
   the region will determine how many of each facility type are required 
   and then determine, facility by facility, which of its institutions 
   are available to build each facility. 
    
   @section intro Introduction 
   The BuildRegion is a region type in Cyclus that is associated with a 
   list of allowed facilities. Any institution in that region must have 
   only those facilities. It is instantiated at the beginning of the 
   simulation and persists until the end. During the course of the 
   simulation, facilities are built according to a demand supplied via 
   input. 
    
   @section modelParams Model Parameters 
   BuildRegion behavior is comprehensively defined by the following 
   parameters: 
    
   -# General Build Information (who can build what) 
     - set <Model*> allowedfacilities: The facilities which are 
     allowed within this region. (** defined in RegionModel.h **)    
     - map <CapacityType : set <Model*> > CapacitySastisfiers: A map of 
     [capacity types] to the [prototypes] that satisfy each capacity 
     type. 
     - map <Model* : set <Model*> > Builders: A map of [prototypes] to 
     the [children] of this region that are capable of building each 
     prototype.
   -# Prototype Build Information (when the number of prototypes is 
   declared explicitly) 
     - pair <Model*,int> PrototypeDemand: A pair of [prototypes] to the 
     [number demanded] to be built at a given time.
     - pair <int, PrototypeDemand> PrototypeBuildOrder: A pair of the 
     [time] at which a particular [PrototypeDemand] has been issued.
     - list <PrototypeBuildOrder> PrototypeOrders: A list of all 
     the orders for specific prototypes to be built.
   -# Capacity Build Information (when a given capacity demand is 
   declared explicitly) 
     - pair <CapacityType,double> CapacityDemand: A pair of 
     [CapacityTypes] to the [total amount] required for this region at a 
     given time.
     - pair <int, CapacityDemand> CapacityBuildOrder: A pair of the 
     [time] at which a particular [CapacityDemand] has been issued.
     - list <CapacityBuildOrder> CapacityBuildOrders: A list of all the 
     orders for specific capacities to be met 
    
   @section behavior Detailed Behavior 
   The BuildRegion is initiated at the beginning of the simulation and 
   persists until the end of the simulation. When it receives a message, 
   the BuildRegion simply passes that message either up the hierarchy to 
   the market for which it was intended or down to the appropriate 
   institution on the path to the recipient. 
    
   The BuildRegion builds prototypes based on demand as a function of 
   time. Demand can come in two forms: demand for a specific prototype 
   or demand for a specific capacity to be met. All building occurs 
   during build phase of the region's tick step; the tick is then passed 
   on to its children. 
    
   During the build phase, the BuildRegion first meets any specific 
   prototype demand that it may have in PrototypeBuildOrders, regardless 
   of any capacity demands or constraints. Next, it inquires about any 
   capacity demand it may have in its CapacityBuildOrders. In this way, 
   capacity is minimally achieved (if there existed an excess demand 
   before the first step of the build phase and this demand was met 
   during the first step, there is no need to build additional 
   prototypes in the second step). 
    
   During the second step of the build phase, if excess demand exists, 
   the BuildRegion makes a decision as to which prototypes to build. 
   This decision is made by ordering the minimum number of prototypes 
   required via a greedy algorithm. Then, for each prototype to be built 
   to meet this capacity demand, the BuildRegion makes another decision 
   as to which institution will build that prototype. This decision is 
   made greedily by selecting the first institution avialable in its 
 */

class BuildRegion : public RegionModel  
{
/* --------------------
 * all MODEL classes have these members
 * --------------------
 */
 public:
  /**
     The default constructor for the BuildRegion 
   */
  BuildRegion();

  /**
     The default destructor for the BuildRegion 
   */
  virtual ~BuildRegion() {};

  /**
     Initalize the BuildRegion from xml. Calls the init function. 
     
     @param cur the curren xml node pointer 
   */
  virtual void init(xmlNodePtr cur);

  /**
     initialize an object by copying another 
   */
  virtual void copy(BuildRegion* src);

  /**
     This drills down the dependency tree to initialize all relevant 
     parameters/containers. 
     Note that this function must be defined only in the specific model 
     in question and not in any inherited models preceding it. 
      
     @param src the pointer to the original (initialized ?) model to be 
   */
  virtual void copyFreshModel(Model* src) { 
    copy(dynamic_cast<BuildRegion*>(src)); 
  }
  
  /**
     print information about the region 
   */
  virtual std::string str();

/* ------------------- */ 


/* --------------------
 * all COMMUNICATOR classes have these members
 * --------------------
 */
 public:
  /**
     Each region is prompted to do its beginning-of-time-step stuff at 
     the tick of the timer. The default behavior is to ignore the tick. 
      
     @param time is the time to perform the tick 
   */
  virtual void handleTick(int time);

/* -------------------- */


/* --------------------
 * the BuildRegion class has these members
 * --------------------
 */
 protected:
  /**
     a container relating each prototype with a set of models (children)
     that can build that prototype
   */
  std::map<Model*, std::list<Model*>*>* builders_;

  /**
     a container of prototype orders, sorted by the order times
   */
  PrototypeOrders* prototypeOrders_;

  /**
     constructs an order given a prototype, number, and time
   */
  PrototypeBuildOrder* constructOrder(Model* prototype, int number, int time);

  /**
     constructs a set of orders from xml and calls the general
     populateOrders() function.

     @param cur the xml node corresponding to the demand for a prototype
  */
  void populateOrders(xmlNodePtr cur);

  /**
     populates prototypeOrders_ given a set of orders. for each order 
     in orders, it calls addOrder().
  */
  void populateOrders(PrototypeOrders* orders);

  /**
     add a specific order to prototypeOrders_
   */
  void addOrder(PrototypeBuildOrder* b) { prototypeOrders_->push_back(b); }
    
  /**
     sort the prototypeOrders_ by time
   */
  void sortOrders();
  
  /**
     populates the builders_ memeber. this method is called after the
     region initializes its children so that they may populate their
     available prototypes members.
  */
  void populateBuilders();
  
  /**
     Given some number of prototypes have been ordered, determine which
     child will build each prototype, and issue the order to build.

     This function calls availableBuilders(), selectBuilder() and 
     placeOrder() for each prototype in the order.
   */
  void handlePrototypeOrder(PrototypeDemand order);

  /**
     determine which builders are available to build a given prototype,
     populating a list of bidders

     @param prototype the prototype to be build
     @param bidders a reference to a list of bidders to be populated
   */
  void getAvailableBuilders(Model* prototype, 
                            std::list<ModelIterator>& bidders);

  /**
     Determine which builder among the bidders will build a 
     given prototype.

     This function will allow for decision making analysis. In its 
     simplest form, it determines which bidder, amongst all bidders, has
     the highest score in the set of builders for the given prototype. 
     That builder is chosen and moved to the bottom of that prototype's 
     set.

     Because the set of bidders is constructed by iterating over the
     list of all possible builders, the first bidder is guaranteed to 
     have the highest score.

     @param prototype the prototype to be built
     @param bidders a list of available builders of the prototype
     
     @return the builder selected
   */
  Model* selectBuilder(Model* prototype, 
                       std::list<ModelIterator>& bidders);

  /**
     place an order for a prototype with a builder
   */
  void placeOrder(Model* prototype, Model* builder);

 public:
  /**
     return a pointer to the front build order
   */
  PrototypeBuildOrder* frontOrder() { return prototypeOrders_->front(); }

  /**
     return a pointer to the back build order
   */
  PrototypeBuildOrder* backOrder() { return prototypeOrders_->back(); }

/* ------------------- */ 

};

#endif
