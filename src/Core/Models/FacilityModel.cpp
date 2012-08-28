// FacilityModel.cpp
// Implements the FacilityModel class

#include "FacilityModel.h"

#include "Timer.h"
#include "BookKeeper.h"
#include "QueryEngine.h"
#include "InstModel.h"

#include <stdlib.h>
#include <iostream>
#include "Logger.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
FacilityModel::FacilityModel() {
  setModelType("Facility");
  in_commods_ = std::vector<std::string>();
  out_commods_ = std::vector<std::string>();
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
FacilityModel::~FacilityModel() {};
  
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void FacilityModel::initCoreMembers(QueryEngine* qe) {
  Model::initCoreMembers(qe);

  // get lifetime and set decommission date
  try {
    fac_lifetime_ = atoi(qe->getElementContent("lifetime").c_str());
  }
  catch (CycNullQueryException e) {
    fac_lifetime_ = TI->simDur();
  }
  setDecommissionDate(TI->time());

  // get the incommodities
  std::string commod;
  try {
    int numInCommod = qe->nElementsMatchingQuery("incommodity");
    for (int i=0;i<numInCommod;i++){
      commod = qe->getElementContent("incommodity",i);
      in_commods_.push_back(commod);
      LOG(LEV_DEBUG2, "none!") << "Facility " << ID() << " has just added incommodity" << commod;
    }
  }
  catch (CycNullQueryException e) {
  }

  // get the outcommodities
  try {
    int numOutCommod = qe->nElementsMatchingQuery("outcommodity");
    for (int i=0;i<numOutCommod;i++){
      commod = qe->getElementContent("outcommodity",i);
      out_commods_.push_back(commod);
      LOG(LEV_DEBUG2, "none!") << "Facility " << ID() << " has just added outcommodity" << commod;
    }
  }
  catch (CycNullQueryException e) {
  }


} 
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void FacilityModel::enterSimulation(Model* parent) {
  Model::enterSimulation(parent);
  initializeConcreteMembers();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void FacilityModel::initializeConcreteMembers() {};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::string FacilityModel::str() {
  return Model::str();
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
InstModel* FacilityModel::facInst() {
  return dynamic_cast<InstModel*>( parent() );
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
  if (time >= decommission_date_) {
    decommission();
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void FacilityModel::handleDailyTasks(int time, int day){
  // facilities who have more intricate details should utilize this function
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void FacilityModel::decommission() {
  delete this;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void FacilityModel::setBuildDate(int current_time) {
  build_date_ = current_time;
  setDecommissionDate(build_date_ + fac_lifetime_);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void FacilityModel::setDecommissionDate(int time) {
    decommission_date_ = time;
}
