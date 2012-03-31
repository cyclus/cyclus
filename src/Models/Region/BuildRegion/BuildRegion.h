// BuildRegion.h
#if !defined(_BUILDREGION_H)
#define _BUILDREGION_H

#include "RegionModel.h"

/**
   The BuildRegion class inherits from the RegionModel class and is 
   dynamically loaded by the Model class when requested.
   
   This region determines if there is a need to meet a certain capacity
   (as defined via input) at each time step. If there is such a need, the
   region will determine how many of each facility type are required and then
   determine, facility by facility, which of its institutions are available
   to build each facility.
   
   @section intro Introduction
   The BuildRegion is a region type in Cyclus that is associated with a list 
   of allowed facilities. Any institution in that region must have only those 
   facilities. It is instantiated at the beginning of the simulation and 
   persists until the end. During the course of the simulation, facilities are 
   built according to a demand supplied via input.
   
   @section modelParams Model Parameters
   BuildRegion behavior is comprehensively defined by the following parameters:

   -# General Build Information (who can build what)
     - vector <Model*> allowedfacilities: The facilities which are allowed within 
     this region.

     - map <CapacityType : set<Model*> > CapacitySastisfiers: A map of [capacity
     types] to the [prototypes] that satisfy each capacity type.

     - map <Model* : set<Model*> > Builders: A map of [prototypes] to the [children]
     of this region that are capable of building each prototype

   -# Prototype Build Information (when the number of prototypes is declared
   explicitly)
     - pair <Model*,int> PrototypeDemand: A pair of [prototypes] to the [number 
     demanded] to be built at a given time
   
     - pair <int, PrototypeDemand> PrototypeBuildOrder: A pair of the [time] at 
     which a particular [PrototypeDemand] has been issued

     - list <PrototypeBuildOrder> PrototypeBuildOrders: A list of all the orders
     for specific prototypes to be built
   
   -# Capacity Build Information (when a given capacity demand is declared
   explicitly)
     - pair <CapacityType,double> CapacityDemand: A pair of [CapacityTypes] to 
     the [total amount] required for this region at a given time
   
     - pair <int, CapacityDemand> CapacityBuildOrder: A pair of the [time] at 
     which a particular [CapacityDemand] has been issued

     - list <CapacityBuildOrder> CapacityBuildOrders: A list of all the orders
     for specific capacities to be met

   @section behavior Detailed Behavior
   The BuildRegion is initiated at the beginning of the simulation and 
   persists until the end of the simulation. When it receives a message, the 
   BuildRegion simply passes that message either up the hierarchy to the 
   market for which it was intended or down to the appropriate institution on 
   the path to the recipient.

   The BuildRegion builds prototypes based on demand as a function of time. Demand
   can come in two forms: demand for a specific prototype or demand for a specific
   capacity to be met. All building occurs during build phase of the region's tick 
   step; the tick is then passed on to its children.

   During the build phase, the BuildRegion first meets any specific prototype demand
   that it may have in PrototypeBuildOrders, regardless of any capacity demands or
   constraints. Next, it inquires about any capacity demand it may have in its 
   CapacityBuildOrders. In this way, capacity is minimally achieved (if there existed
   an excess demand before the first step of the build phase and this demand was met
   during the first step, there is no need to build additional prototypes in the second
   step).

   During the second step of the build phase, if excess demand exists, the BuildRegion
   makes a decision as to which prototypes to build. This decision is made by ordering
   the minimum number of prototypes required via a greedy algorithm. Then, for each
   prototype to be built to meet this capacity demand, the BuildRegion makes another
   decision as to which institution will build that prototype. This decision is made
   greedily by selecting the first institution avialable in its list of Builders. That
   institution is then sent to the bottom of its Builders list.
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
  BuildRegion() {};

  /**
     The default destructor for the BuildRegion
   */
  virtual ~BuildRegion() {};
   
  /**
     initialize an object from XML input
   */
  virtual void init(xmlNodePtr cur)  { RegionModel::init(cur); };

  /**
     initialize an object by copying another
   */
  virtual void copy(BuildRegion* src) { RegionModel::copy(src); } ;

  /**
     This drills down the dependency tree to initialize all relevant parameters/containers.
     
     Note that this function must be defined only in the specific model in question and not in any 
     inherited models preceding it.
     
     @param src the pointer to the original (initialized ?) model to be copied
   */
  virtual void copyFreshModel(Model* src){ copy(dynamic_cast<BuildRegion*>(src)); };
  
  /**
     print information about the region
   */
  virtual void print()               { RegionModel::print();   } ;

/* ------------------- */ 


/* --------------------
 * all COMMUNICATOR classes have these members
 * --------------------
 */
 public:
  /**
     Each region is prompted to do its beginning-of-time-step
     stuff at the tick of the timer.
     The default behavior is to ignore the tick.
     
     @param time is the time to perform the tick
  */
  virtual void handleTick(int time);

/* -------------------- */


/* --------------------
 * the BuildRegion class have these members
 * --------------------
 */
  
/* ------------------- */ 

};

#endif
