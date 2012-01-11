// FacilityModel.cpp
// Implements the FacilityModel class

#include "FacilityModel.h"

#include "Timer.h"
#include "Logician.h"
#include "BookKeeper.h"
#include "InputXML.h"

#include <iostream>
#include "Logger.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
FacilityModel::FacilityModel() {
  setModelType("Facility");
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
FacilityModel::~FacilityModel() {};
  
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void FacilityModel::init(xmlNodePtr cur) {
  Model::init(cur);

  // Specific initialization for FacilityModels
  xmlNodeSetPtr nodes = XMLinput->get_xpath_elements(cur, "/simulation/region/institution");
   
  for (int i=0;i<nodes->nodeNr;i++){
    // MJGFLAG this inst name stuff needs to be depricated
    inst_name_ = XMLinput->get_xpath_content(nodes->nodeTab[i], "name");
    this->setInstName(inst_name_);
    LOG(LEV_DEBUG2) << "Facility " << ID() << " has just set its inst to " << inst_name_;
  }
} 

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void FacilityModel::copy(FacilityModel* src) { 
  Model::copy(src); 
  Communicator::copy(src); 

  // don't copy fac_name to new instance
  this->setFacName("");

  LI->addModel(this, FACILITY);
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
InstModel* FacilityModel::getFacInst() {
  return dynamic_cast<InstModel*>( parent() );
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void FacilityModel::handlePreHistory() {
  // facilities should override this method, unless they're very naiive.
  // this function allows the facility to set up the simulation before it begins.
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

  // if its the last month, decommission the facility
  if (TI->checkEndMonth()) {this->decommission();}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void FacilityModel::handleDailyTasks(int time, int day){
  // facilities who have more intricate details should utilize this function
}

