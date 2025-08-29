// Implements the Facility class
#include "facility.h"

#include <limits>
#include <sstream>
#include <stdlib.h>

#include "error.h"
#include "infile_tree.h"
#include "institution.h"
#include "logger.h"
#include "timer.h"

namespace cyclus {

Facility::Facility(Context* ctx) : Trader(this), Agent(ctx) {
  kind_ = std::string("Facility");
}

Facility::~Facility() {}

void Facility::InitFrom(Facility* m) {
  Agent::InitFrom(m);
}

void Facility::Build(Agent* parent) {
  Agent::Build(parent);
}

void Facility::EnterNotify() {
  Agent::EnterNotify();
  context()->RegisterTrader(dynamic_cast<Trader*>(this));
  context()->RegisterTimeListener(this);
}

std::string Facility::str() {
  std::stringstream ss("");
  ss << Agent::str() << " with: " << " lifetime: " << lifetime()
     << " build date: " << enter_time();
  return ss.str();
}

void Facility::Decommission() {
  if (!CheckDecommissionCondition()) {
    throw Error("Cannot decommission " + prototype());
  }

  context()->UnregisterTrader(dynamic_cast<Trader*>(this));
  context()->UnregisterTimeListener(this);
  Agent::Decommission();
}

bool Facility::CheckDecommissionCondition() {
  return true;
}

Region* Facility::GetRegion(int layer) {
  return dynamic_cast<Region*>(GetAncestorOfKind("Region", layer));
}

Institution* Facility::GetInstitution(int layer) {
  return dynamic_cast<Institution*>(GetAncestorOfKind("Inst", layer));
}

}  // namespace cyclus
