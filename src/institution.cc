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

Institution::~Institution() {}

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
  std::set<Agent*>::iterator it;
  for (it = children().begin(); it != children().end(); ++it) {
    Agent* a = *it;
    if (a->lifetime() != -1 && context()->time() >= a->exit_time()) {
      Facility* fac = dynamic_cast<Facility*>(a);
      if (fac == NULL || fac->CheckDecommissionCondition()) {
        CLOG(LEV_INFO3) << a->prototype()
                        << " has reached the end of its lifetime";
        context()->SchedDecom(a);
      }
    }
  }
}

Region* Institution::GetRegion() {
  return dynamic_cast<Region*>(GetAncestorOfKind("Region"));
}

}  // namespace cyclus
