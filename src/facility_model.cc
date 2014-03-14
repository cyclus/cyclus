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

FacilityModel::FacilityModel(Context* ctx)
    : Trader(this),
      Model(ctx) {
  kind_ = "Facility";
};

FacilityModel::~FacilityModel() {};

void FacilityModel::InitFrom(FacilityModel* m) {
  Model::InitFrom(m);
}

void FacilityModel::Build(Model* parent) {
  Model::Build(parent);
}

void FacilityModel::DoRegistration() {
  context()->RegisterTrader(this);
  context()->RegisterTimeListener(this);
}

std::string FacilityModel::str() {
  std::stringstream ss("");
  ss << Model::str() << " with: "
     << " lifetime: " << lifetime()
     << " build date: " << enter_time();
  return ss.str();
};

void FacilityModel::Decommission() {
  if (!CheckDecommissionCondition()) {
    throw Error("Cannot decommission " + prototype());
  }

  context()->UnregisterTrader(this);
  context()->UnregisterTimeListener(this);
  Model::Decommission();
}

bool FacilityModel::CheckDecommissionCondition() {
  return true;
}

} // namespace cyclus
