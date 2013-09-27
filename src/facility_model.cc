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
      fac_lifetime_(std::numeric_limits<int>::max()) {
  SetModelType("Facility");
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
FacilityModel::~FacilityModel() {};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void FacilityModel::InitCoreMembers(QueryEngine* qe) {
  Model::InitCoreMembers(qe);

  // get lifetime
  int lifetime =
      cyclus::GetOptionalQuery<int>(qe, "lifetime", context()->sim_dur());
  SetFacLifetime(lifetime);

  // get the incommodities
  std::string commod;
  int numInCommod = qe->NElementsMatchingQuery("incommodity");
  for (int i = 0; i < numInCommod; i++) {
    commod = qe->GetElementContent("incommodity", i);
    in_commods_.push_back(commod);
    LOG(LEV_DEBUG2, "none!") << "Facility " << id()
                             << " has just added incommodity" << commod;
  }

  // get the outcommodities
  int numOutCommod = qe->NElementsMatchingQuery("outcommodity");
  for (int i = 0; i < numOutCommod; i++) {
    commod = qe->GetElementContent("outcommodity", i);
    out_commods_.push_back(commod);
    LOG(LEV_DEBUG2, "none!") << "Facility " << id()
                             << " has just added outcommodity" << commod;
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void FacilityModel::InitFrom(Model* m) {
  Model::InitFrom(m);
  FacilityModel* fm = dynamic_cast<FacilityModel*>(m);
  fac_lifetime_ = fm->fac_lifetime_;
  in_commods_ = fm->in_commods_;
  out_commods_ = fm->out_commods_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::string FacilityModel::str() {
  std::stringstream ss("");
  ss << Model::str() << " with: "
     << " lifetime: " << FacLifetime()
     << " build date: " << birthtime()
     << " decommission date: " << birthtime() + fac_lifetime_;
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
  delete this;
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
  return (time >= birthtime() + fac_lifetime_);
}

} // namespace cyclus
