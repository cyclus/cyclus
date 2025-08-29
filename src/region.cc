// Implements the Region class
#include "region.h"

#include <iostream>
#include <string>

#include "agent.h"
#include "error.h"
#include "infile_tree.h"
#include "institution.h"
#include "logger.h"
#include "timer.h"

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

void Region::EnterNotify() {
  context()->RegisterTimeListener(this);
}

void Region::Decommission() {
  context()->UnregisterTimeListener(this);
  Agent::Decommission();
}

std::string Region::str() {
  std::string s = Agent::str();

  s += " has insts: ";
  for (std::set<Agent*>::const_iterator inst = children().begin();
       inst != children().end();
       inst++) {
    s += (*inst)->prototype() + ", ";
  }
  return s;
}

}  // namespace cyclus
