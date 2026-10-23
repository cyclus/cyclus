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
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>

namespace cyclus {

Facility::Facility(Context* ctx) : Trader(this), Agent(ctx), schedule_helper_(this) {
  kind_ = std::string("Facility");
}

Facility::~Facility() {}

void Facility::InitFrom(Facility* m) {
  Agent::InitFrom(m);
}

void Facility::Build(Agent* parent) {
  Agent::Build(parent); 
  toolkit::CommodityRecipeContext comd;
  if (lifetime() >= 0 && CheckDecommissionCondition() == NULL) { 
    context()->SchedDecom(this, exit_time());
  }
  for (auto& requests: GetMatlRequests()) {
    if(requests){
      for(auto& request : requests->requests()) {
      std::string commodity = request->commodity();
      context()->RegisterCommodityConsumer(commodity,this);
      comd.AddInCommod(commodity,"b","t","x");
      }
    }
  }
  comd.CommodSnapshot(DbInit(this)); 
  context()-> RegisterCommoditiesTraded(context()->time(), GetInCommods());
}

void Facility::EnterNotify() {
  Agent::EnterNotify();
  context()->RegisterTrader(dynamic_cast<Trader*>(this));
  context()->RegisterTimeListener(this);
  context()->SetTraderIds(id(),dynamic_cast<Trader*>(this));

  schedule_helper_.InitialTrade();
}

std::string Facility::str() {
  std::stringstream ss("");
  ss << Agent::str() << " with: " << " lifetime: " << lifetime()
     << " build date: " << enter_time();
  return ss.str();
}

void Facility::Decommission() {
  // if (CheckDecommissionCondition() == NULL){
  // std::cout << CheckDecommissionCondition() << std::endl;}
  // if (CheckDecommissionCondition() == false) { //check what happens w NULL
  //   throw Error("Cannot decommission " + prototype());
  // }
  context()->UnregisterTrader(dynamic_cast<Trader*>(this));
  context()->UnregisterTimeListener(this); 
  context()->UnregisterCommodityConsumer(GetInCommods(),this);

  Agent::Decommission();
}

bool Facility::CheckDecommissionCondition() {
  return NULL;
}

void Facility::Tock(){ // archetype developers need to invoke this method in tock 
  EventRequest();
}

void Facility::Tick(){
  //this gets toggled if a trade is completed 
  std::cout << info().cycle_length << std::endl;
 SetParticipated(false); //archetype developers need to invoke this method in tick
}

void Facility::EventRequest(){
  if (ReturnParticipated()){ // should maybe add this to the tock metho instead of event request
    schedule_helper_.ScheduleMethod(info());
  }
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
