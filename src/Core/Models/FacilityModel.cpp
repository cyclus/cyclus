// FacilityModel.cpp
// Implements the FacilityModel class

#include "FacilityModel.h"

#include "Timer.h"
#include "BookKeeper.h"
#include "QueryEngine.h"
#include "InstModel.h"

#include <stdlib.h>
#include <sstream>
#include "Logger.h"
#include <limits>

using namespace std;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
FacilityModel::FacilityModel() : 
  fac_lifetime_(numeric_limits<int>::max()),
  decommission_date_(numeric_limits<int>::max()) {
  setModelType("Facility");
  in_commods_ = std::vector<std::string>();
  out_commods_ = std::vector<std::string>();
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
FacilityModel::~FacilityModel() {};
  
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void FacilityModel::initCoreMembers(QueryEngine* qe) {
  Model::initCoreMembers(qe);

  // get lifetime
  try {
    setFacLifetime(atoi(qe->getElementContent("lifetime").c_str()));
  }
  catch (CycNullQueryException e) {
    setFacLifetime(TI->simDur());
  }
  
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
Prototype* FacilityModel::clone() {
  FacilityModel* clone = dynamic_cast<FacilityModel*>(Model::constructModel(modelImpl()));
  clone->cloneCoreMembersFrom(this);
  clone->cloneModuleMembersFrom(this);
  clone->setBuildDate(TI->time());
  CLOG(LEV_DEBUG3) << clone->modelImpl() << " cloned: " << clone->str();
  CLOG(LEV_DEBUG3) << "               From: " << this->str();
  return clone;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void FacilityModel::cloneCoreMembersFrom(FacilityModel* source) {
  setName(source->name());
  setModelImpl(source->modelImpl());
  setModelType(source->modelType());
  setFacLifetime(source->facLifetime());
  in_commods_ = source->inputCommodities();
  out_commods_ = source->outputCommodities();  
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void FacilityModel::cloneModuleMembersFrom(FacilityModel* source) {}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::string FacilityModel::str() {
  stringstream ss("");
  ss << Model::str() << " with: "
     << " lifetime: " << facLifetime()
     << " build date: " << build_date_
     << " decommission date: " << decommission_date_;
  return ss.str();
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
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void FacilityModel::handleDailyTasks(int time, int day){
  // facilities who have more intricate details should utilize this function
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void FacilityModel::decommission() {
  if (!checkDecommissionCondition()) 
    throw CycOverrideException("Cannot decommission " + name());

  CLOG(LEV_INFO3) << name() << " is being decommissioned";
  deleteModel(this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool FacilityModel::checkDecommissionCondition() {
  return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::vector<std::string> FacilityModel::inputCommodities() {
  return in_commods_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::vector<std::string> FacilityModel::outputCommodities() {
  return out_commods_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool FacilityModel::lifetimeReached() {
  return (TI->time() >= decommission_date_);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void FacilityModel::setBuildDate(int current_time) {
  build_date_ = current_time;
  setDecommissionDate(build_date_ + fac_lifetime_);
  CLOG(LEV_DEBUG3) << name() << " has set its time-related members: ";
  CLOG(LEV_DEBUG3) << " * lifetime: " << fac_lifetime_; 
  CLOG(LEV_DEBUG3) << " * build date: " << build_date_; 
  CLOG(LEV_DEBUG3) << " * decommisison date: " << decommission_date_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void FacilityModel::setDecommissionDate(int time) {
  double current_time = TI->time();
  double final_time = TI->finalTime();
  if (time + current_time < final_time)
    {
      decommission_date_ = time;
    }
  else
    {
      decommission_date_ = final_time;
    }
}
