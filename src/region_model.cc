// Regionmodel.cc
// Implements the Region class

#include <string>
#include <iostream>

#include "region_model.h"

#include "model.h"
#include "inst_model.h"
#include "error.h"
#include "timer.h"
#include "logger.h"
#include "query_engine.h"

namespace cyclus {

void Region::InitFrom(Region* m) {
  Agent::InitFrom(m);
}

Region::Region(Context* ctx) : Agent(ctx) {
  kind_ = "Region";
}

void Region::Build(Agent* parent) {
  Agent::Build(parent);
}

void Region::DoRegistration() {
  context()->RegisterTimeListener(this);
}

void Region::Decommission() {
  context()->UnregisterTimeListener(this);
  Agent::Decommission();
}

std::string Region::str() {
  std::string s = Agent::str();

  s += " has insts: ";
  for (std::vector<Agent*>::const_iterator inst = children().begin();
       inst != children().end();
       inst++) {
    s += (*inst)->prototype() + ", ";
  }
  return s;
}

} // namespace cyclus
