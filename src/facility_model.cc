// Facilitymodel.cc
// Implements the FacilityAgent class

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

FacilityAgent::FacilityAgent(Context* ctx)
    : Trader(this),
      Agent(ctx) {
  kind_ = "Facility";
};

FacilityAgent::~FacilityAgent() {};

void FacilityAgent::InitFrom(FacilityAgent* m) {
  Agent::InitFrom(m);
}

void FacilityAgent::Build(Agent* parent) {
  Agent::Build(parent);
}

void FacilityAgent::DoRegistration() {
  context()->RegisterTrader(this);
  context()->RegisterTimeListener(this);
}

std::string FacilityAgent::str() {
  std::stringstream ss("");
  ss << Agent::str() << " with: "
     << " lifetime: " << lifetime()
     << " build date: " << enter_time();
  return ss.str();
};

void FacilityAgent::Decommission() {
  if (!CheckDecommissionCondition()) {
    throw Error("Cannot decommission " + prototype());
  }

  context()->UnregisterTrader(this);
  context()->UnregisterTimeListener(this);
  Agent::Decommission();
}

bool FacilityAgent::CheckDecommissionCondition() {
  return true;
}

} // namespace cyclus
