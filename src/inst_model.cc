/// Instmodel.cc
// Implements the InstAgent class

#include <iostream>
#include <sstream>
#include <string>

#include "inst_model.h"

#include "logger.h"
#include "timer.h"
#include "error.h"
#include "facility_model.h"
#include "query_engine.h"

namespace cyclus {

InstAgent::InstAgent(Context* ctx) : Agent(ctx) {
  kind_ = "Inst";
}

void InstAgent::InitFrom(InstAgent* m) {
  Agent::InitFrom(m);
}

std::string InstAgent::str() {
  if (parent() != NULL) {
    return Agent::str() + " in region" + parent()->prototype();
  } else {
    return Agent::str() + " with no region.";
  }
}

void InstAgent::Build(Agent* parent) {
  Agent::Build(parent);
}

void InstAgent::DoRegistration() {
  context()->RegisterTimeListener(this);
}

void InstAgent::Decommission() {
  context()->UnregisterTimeListener(this);
  Agent::Decommission();
}

void InstAgent::Tock(int time) {
  std::vector<Agent*> to_decomm;
  for (int i = 0; i < children().size(); i++) {
    FacilityAgent* child = dynamic_cast<FacilityAgent*>(children().at(i));
    int lifetime = child->lifetime();
    if (lifetime != -1 && time >= child->enter_time() + lifetime) {
      CLOG(LEV_INFO3) << child->prototype() << " has reached the end of its lifetime";
      if (child->CheckDecommissionCondition()) {
        to_decomm.push_back(child);
      }
    }
  }

  while (!to_decomm.empty()) {
    Agent* child = to_decomm.back();
    to_decomm.pop_back();
    context()->SchedDecom(child);
  }
}

} // namespace cyclus
