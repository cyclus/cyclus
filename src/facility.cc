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
  if (lifetime() >= 0 && CheckDecommissionCondition() == NULL) { 
    context()->SchedDecom(this, exit_time());
  }
}

void Facility::EnterNotify() {
  Agent::EnterNotify();
  context()->RegisterTrader(dynamic_cast<Trader*>(this));
  context()->RegisterTimeListener(this);
  // maybe have a context()->RegisterCommodityConsumer(this); 
}

std::string Facility::str() {
  std::stringstream ss("");
  ss << Agent::str() << " with: " << " lifetime: " << lifetime()
     << " build date: " << enter_time();
  return ss.str();
}

void Facility::Decommission() {
  if (!CheckDecommissionCondition()) { //check what happens w NULL
    throw Error("Cannot decommission " + prototype());
  }

  context()->UnregisterTrader(dynamic_cast<Trader*>(this));
  context()->UnregisterTimeListener(this);
  Agent::Decommission();
}

bool Facility::CheckDecommissionCondition() {
  return NULL;
}

void Facility::Tock(){
  EventRequest();
}

void Facility::Tick(){
  SetTraded(false);
}

Region* Facility::GetParentRegion(int layer) {
  return dynamic_cast<Region*>(GetAncestorOfKind("Region", layer));
}

Institution* Facility::GetParentInstitution(int layer) {
  return dynamic_cast<Institution*>(GetAncestorOfKind("Inst", layer));
}

Facility* Facility::GetParentFacility(int layer) {
  return dynamic_cast<Facility*>(GetAncestorOfKind("Facility", layer));
}

std::vector<Region*> Facility::GetAllParentRegions() {
  return GetAllAncestorsOfType<Region>("Region");
}

std::vector<Institution*> Facility::GetAllParentInstitutions() {
  return GetAllAncestorsOfType<Institution>("Inst");
}

std::vector<Facility*> Facility::GetAllParentFacilities() {
  return GetAllAncestorsOfType<Facility>("Facility");
}

}  // namespace cyclus
