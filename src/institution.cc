// Implements the Institution class
#include "institution.h"

#include <iostream>
#include <sstream>
#include <string>

#include "error.h"
#include "facility.h"
#include "infile_tree.h"
#include "logger.h"
#include "timer.h"

namespace cyclus {

Institution::Institution(Context* ctx) : Agent(ctx) {
  kind_ = "Inst";
}

Institution::~Institution() {
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

void Institution::EnterNotify() {
  context()->RegisterTimeListener(this);
}

void Institution::Decommission() {
  context()->UnregisterTimeListener(this);
  Agent::Decommission();
}

void Institution::Tock() {
  std::vector<Agent*> to_decomm;
  std::set<Agent*>::iterator it;
  // set children's parents to NULL
  for (it = children().begin(); it != children().end(); ++it) {
    Facility* child = dynamic_cast<Facility*>(*it);
    int lifetime = child->lifetime();
    if (lifetime != -1 && context()->time() >= child->enter_time() + lifetime) {
      CLOG(LEV_INFO3) << child->prototype()
                      << " has reached the end of its lifetime";
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

}  // namespace cyclus
