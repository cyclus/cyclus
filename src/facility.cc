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
  //for agents WITHOUT the need for a checkdecom status, they can easily schedule decom at build (only reactor/separations use this right now)
  if (lifetime() >= 0 && CheckDecommissionCondition() == NULL) { 
    context()->SchedDecom(this, exit_time());
  }
  for (auto& requests: GetMatlRequests()) {
    if(requests){
      for(auto& request : requests->requests()) {
  // //hopefully this is a dry run with no impact on DRE (ie adding porfolios)
      std::string commodity = request->commodity();
      context()->RegisterCommodityConsumer(commodity,this);
      FillInCommods(commodity); //any repeats should be
      }
    }
  }
  std::cout<<"Agent of type" << prototype()<< " has " << GetInCommods().size()<<" in_commods_ size in Trader \n\n\n";
  // FORCEFULLY SET THE COMMOD MAP (not relying on tick/tock event request)
  context()-> RegisterCommoditiesTraded(context()->time(), GetInCommods());
  std::cout <<"Context commodity map size called in Facility::Build is "<< (context()->CommoditiesTraded(context()->time())).size()<<" (not empty) \n\n\n\n";
}

void Facility::EnterNotify() {
  Agent::EnterNotify();
  context()->RegisterTrader(dynamic_cast<Trader*>(this));
  context()->RegisterTimeListener(this);
  schedule_helper_.InitialTrade(); 
  //all agents who have been build should want to immediately trade 
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
  SetTraded(false); //archetype developers need to invoke this method in tick
}

void Facility::EventRequest(){
  schedule_helper_.FixIncSchedule(); //FixIncSchedule schedules like cyclus 1.6v 
  for(int i: schedule_helper_.EventTime()){ //probably needed in future... 
    selftimes_.insert(i);
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
