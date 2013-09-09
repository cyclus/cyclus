// Facilitymodel.cc
// Implements the FacilityModel class

#include "facility_model.h"

#include "timer.h"
#include "query_engine.h"
#include "inst_model.h"
#include "error.h"

#include <stdlib.h>
#include <sstream>
#include "logger.h"
#include <limits>

namespace cyclus {

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
FacilityModel::FacilityModel(Context* ctx)
    : TimeAgent(ctx),
      fac_lifetime_(std::numeric_limits<int>::max()),
      decommission_date_(std::numeric_limits<int>::max()),
      build_date_(0) {
  SetModelType("Facility");
  in_commods_ = std::vector<std::string>();
  out_commods_ = std::vector<std::string>();
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
FacilityModel::~FacilityModel() {};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void FacilityModel::InitCoreMembers(QueryEngine* qe) {
  Model::InitCoreMembers(qe);

  // get lifetime
  int lifetime;
  qe->NElementsMatchingQuery("incommodity") == 1 ?
      lifetime = atoi(qe->GetElementContent("lifetime").c_str()) :
      lifetime = context()->sim_dur();
  SetFacLifetime(lifetime);

  // get the incommodities
  std::string commod;
  int numInCommod = qe->NElementsMatchingQuery("incommodity");
  for (int i = 0; i < numInCommod; i++) {
    commod = qe->GetElementContent("incommodity", i);
    in_commods_.push_back(commod);
    LOG(LEV_DEBUG2, "none!") << "Facility " << ID()
                             << " has just added incommodity" << commod;
  }

  // get the outcommodities
  int numOutCommod = qe->NElementsMatchingQuery("outcommodity");
  for (int i = 0; i < numOutCommod; i++) {
    commod = qe->GetElementContent("outcommodity", i);
    out_commods_.push_back(commod);
    LOG(LEV_DEBUG2, "none!") << "Facility " << ID()
                             << " has just added outcommodity" << commod;
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Prototype* FacilityModel::clone() {
  FacilityModel* clone = dynamic_cast<FacilityModel*>(Model::ConstructModel(
                                                        context(),
                                                        ModelImpl()));
  clone->CloneCoreMembersFrom(this);
  clone->CloneModuleMembersFrom(this);
  clone->SetBuildDate(context()->time());
  CLOG(LEV_DEBUG3) << clone->ModelImpl() << " cloned: " << clone->str();
  CLOG(LEV_DEBUG3) << "               From: " << this->str();
  return clone;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void FacilityModel::CloneCoreMembersFrom(FacilityModel* source) {
  SetName(source->name());
  SetModelImpl(source->ModelImpl());
  SetModelType(source->ModelType());
  SetFacLifetime(source->FacLifetime());
  in_commods_ = source->InputCommodities();
  out_commods_ = source->OutputCommodities();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::string FacilityModel::str() {
  std::stringstream ss("");
  ss << Model::str() << " with: "
     << " lifetime: " << FacLifetime()
     << " build date: " << build_date_
     << " decommission date: " << decommission_date_;
  return ss.str();
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
InstModel* FacilityModel::FacInst() {
  return dynamic_cast<InstModel*>(parent());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void FacilityModel::HandleDailyTasks(int time, int day) {
  // facilities who have more intricate details should utilize this function
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void FacilityModel::Decommission() {
  if (!CheckDecommissionCondition()) {
    throw Error("Cannot decommission " + name());
  }

  CLOG(LEV_INFO3) << name() << " is being decommissioned";
  DeleteModel(this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool FacilityModel::CheckDecommissionCondition() {
  return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::vector<std::string> FacilityModel::InputCommodities() {
  return in_commods_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::vector<std::string> FacilityModel::OutputCommodities() {
  return out_commods_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool FacilityModel::LifetimeReached(int time) {
  return (time >= decommission_date_);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void FacilityModel::SetBuildDate(int current_time) {
  build_date_ = current_time;
  SetDecommissionDate(build_date_ + fac_lifetime_ -
                      1); // -1 because you want the decommission to occur on the previous time's tock
  CLOG(LEV_DEBUG3) << name() << " has set its time-related members: ";
  CLOG(LEV_DEBUG3) << " * lifetime: " << fac_lifetime_;
  CLOG(LEV_DEBUG3) << " * build date: " << build_date_;
  CLOG(LEV_DEBUG3) << " * decommisison date: " << decommission_date_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void FacilityModel::SetDecommissionDate(int time) {
  double final_time = context()->start_time() + context()->sim_dur();
  if (time < final_time) {
    decommission_date_ = time;
  } else {
    decommission_date_ = final_time;
  }
  CLOG(LEV_DEBUG3) << name() << " is setting its decommission date: ";
  CLOG(LEV_DEBUG3) << " * Set Time: " << time;
  CLOG(LEV_DEBUG3) << " * Final Time: " << final_time;
  CLOG(LEV_DEBUG3) << " * decommisison date: " << decommission_date_;
}
} // namespace cyclus
