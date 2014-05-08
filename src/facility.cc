// facility.cc
// Implements the Facility class

#include "facility.h"

#include "timer.h"
#include "infile_tree.h"
#include "institution.h"
#include "error.h"

#include <stdlib.h>
#include <sstream>
#include "logger.h"
#include <limits>

namespace cyclus {

Facility::Facility(Context* ctx)
    : Trader(this),
      Agent(ctx) {
  kind_ = "Facility";
};

Facility::~Facility() {};


void Facility::InitFrom(Facility* m) {
  Agent::InitFrom(m);
}

void Facility::Build(Agent* parent) {
  Agent::Build(parent);
}

void Facility::EnterNotify() {
  Agent::EnterNotify();
  context()->RegisterTrader(this);
  context()->RegisterTimeListener(this);
}

std::string Facility::str() {
  std::stringstream ss("");
  ss << Agent::str() << " with: "
     << " lifetime: " << lifetime()
     << " build date: " << enter_time();
  return ss.str();
};

void Facility::Decommission() {
  if (!CheckDecommissionCondition()) {
    throw Error("Cannot decommission " + prototype());
  }

  context()->UnregisterTrader(this);
  context()->UnregisterTimeListener(this);
  Agent::Decommission();
}

bool Facility::CheckDecommissionCondition() {
  return true;
}

} // namespace cyclus
