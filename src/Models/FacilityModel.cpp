// FacilityModel.cpp
// Implements the FacilityModel class

#include "FacilityModel.h"
#include "Logician.h"

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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void FacilityModel::init(xmlNodePtr cur)
{
  Model::init(cur);

  // Specific initialization for FacilityModels
  xmlNodeSetPtr nodes = XMLinput->get_xpath_elements(cur, "/simulation/region/institution");
  
  for (int i=0;i<nodes->nodeNr;i++){
    inst_name = XMLinput->get_xpath_content(nodes->nodeTab[i], "name");
    this->setInstName(inst_name);
    cout << "Facility " << ID << " has just set its inst to " << inst_name << endl;
  }
} 

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void FacilityModel::copy(FacilityModel* src)
{ 
  Model::copy(src); 
  Communicator::copy(src); 

  // don't copy fac_name to new instance
  fac_name = "";


  LI->addFacility(this);
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
InstModel* FacilityModel::getFacInst()
{
  return (InstModel*)(LI->getInstByName(inst_name));
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void FacilityModel::handleTick(int time){
  // facilities should override this method, unless they're very naiive.
  // generally, a facility's handleTick() behavior should be to 
  // offer an amount of its output material equal to its capacity 
  // and to request an amount of raw material also equal to its capacity.
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void FacilityModel::handleTock(int time){
  // facilities should override this method, unless they're very naiive.
  // generally, a faility's handleTock() behavior should be to 
  // process any orders passed down from the market, 
  // send the appropriate materials, 
  // receive any materials the market has found a source for, 
  // and record all material transfers.
}

