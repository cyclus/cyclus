/// Instmodel.cc
// Implements the Institution class

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

Institution::Institution(Context* ctx) : Agent(ctx) {
  kind_ = "Inst";
}

void Institution::InitFrom(Institution* m) {
  Agent::InitFrom(m);
}

std::string Institution::str() {
  if (parent() != NULL) {
    return Agent::str() + " in region" + parent()->prototype();
  } else {
    return Agent::str() + " with no region.";
  }
}

void Institution::Build(Agent* parent) {
  Agent::Build(parent);
}

void Institution::DoRegistration() {
  context()->RegisterTimeListener(this);
}

void Institution::Decommission() {
  context()->UnregisterTimeListener(this);
  Agent::Decommission();
}

void Institution::Tock(int time) {
  std::vector<Agent*> to_decomm;
  for (int i = 0; i < children().size(); i++) {
    Facility* child = dynamic_cast<Facility*>(children().at(i));
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
