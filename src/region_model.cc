// Regionmodel.cc
// Implements the RegionAgent class

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

void RegionAgent::InitFrom(RegionAgent* m) {
  Agent::InitFrom(m);
}

RegionAgent::RegionAgent(Context* ctx) : Agent(ctx) {
  kind_ = "Region";
}

void RegionAgent::Build(Agent* parent) {
  Agent::Build(parent);
}

void RegionAgent::DoRegistration() {
  context()->RegisterTimeListener(this);
}

void RegionAgent::Decommission() {
  context()->UnregisterTimeListener(this);
  Agent::Decommission();
}

std::string RegionAgent::str() {
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
