// FacilityModel.cpp
// Implements the FacilityModel class

#include "FacilityModel.h"

/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

// Initialize the FacilityModel nextID to zero.
int FacilityModel::nextID = 0;

/* --------------------
 * all FACILITYMODEL classes have these members
 * --------------------
 */

void FacilityModel::init(xmlNodePtr cur)
{
  Model::init(cur);

  /** 
   *  Specific initialization for FacilityModels
   */

  fac_name = "";
} 

void FacilityModel::copy(FacilityModel* src)
{ 
  Model::copy(src); 
  Communicator::copy(src); 

  // don't copy fac_name to new instance
  fac_name = "";
};

void FacilityModel::handleTick(int time){
  // facilities should override this method, unless they're very naiive.
  // generic handleTick() behavior should probably be to offer that facility's
  // capacity worth of their output material and request their capacity worth 
  // of their raw material.
}

void FacilityModel::handleTock(int time){
  // facilities should override this method, unless they're very naiive.
  // generic handleTock() behavior should probably be to execute any
  // outstanding orders.  
}

