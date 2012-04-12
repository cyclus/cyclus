// NullRegion.h
#if !defined(_NULLREGION_H)
#define _NULLREGION_H

#include <iostream>

#include "RegionModel.h"

/**
   The NullRegion class inherits from the RegionModel class and is 
   dynamically loaded by the Model class when requested. 
    
   This region will do nothing. This RegionModel never alters any 
   messages transmitted through it or anything else. 
    
   @section intro Introduction 
   The NullRegion is a region type in Cyclus that is associated with a 
   list of allowed facilities. Any institution in that region must have 
   only those facilities. It is instantiated at the beginning of the 
   simulation and persists until the end. 
    
   @section modelParams Model Parameters 
   NullRegion behavior is comprehensively defined by the following 
   parameters: - vector<Model> allowedfacilities: The facilities which 
   are allowed within this region. 
    
   @section optionalParams Optional Parameters 
   NullRegion behavior may also be specified with the following optional 
   parameters which have default values listed here. 
   - string name: A non-generic name for this region. 
    
   @section behavior Detailed Behavior 
   The NullRegion is initiated at the beginning of the simulation and 
   persists until the end of the simulation. When it receives a message, 
   the NullRegion simply passes that message either up the hierarchy to 
   the market for which it was intended or down to the appropriate 
   institution on the path to the recipient. 
 */
class NullRegion : public RegionModel  {
/* --------------------
 * all MODEL classes have these members
 * --------------------
 */
 public:
  /**
     The default constructor for the NullRegion 
   */
  NullRegion() {};

  /**
     The default destructor for the NullRegion 
   */
  virtual ~NullRegion() {};
   
  /**
     initialize an object from XML input 
   */
  virtual void init(xmlNodePtr cur)  { RegionModel::init(cur); };

  /**
     initialize an object by copying another 
   */
  virtual void copy(NullRegion* src) { RegionModel::copy(src); } ;

  /**
     This drills down the dependency tree to initialize all relevant 
     parameters/containers. 
     Note that this function must be defined only in the specific model 
     in question and not in any inherited models preceding it. 
      
     @param src the pointer to the original (initialized ?) model to be 
   */
  virtual void copyFreshModel(Model* src){ copy(dynamic_cast<NullRegion*>(src)); };
  
  /**
     print information about the region 
   */
  virtual std::string str()               { RegionModel::str();   } ;

/* ------------------- */ 

};


#endif
