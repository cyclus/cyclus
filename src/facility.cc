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
  // if (lifetime() >= 0 && CheckDecommissionCondition() == NULL) { 
  //   context()->SchedDecom(this, exit_time());
  // }
  for (auto& requests: GetMatlRequests()) {
    if(requests){
      for(auto& request : requests->requests()) {
  // //hopefully this is a dry run with no impact on DRE (ie adding porfolios)
      std::string commodity = request->commodity();
      context()->RegisterCommodityConsumer(commodity,this);
      FillInCommods(1); //("commodity"); //any repeats should be
      std::cout<<commodity<< "\n\n\n";
      context()->test(17);
      context()->stringtest("velma");
      }
    }
  }
  std::cout<< GetInCommods().size()<<"in fac the incommods in trader is \n\n\n";
  std::cout<<context()->GetStringTest()<<"getting string test in build \n\n\n";
  context()->RegisterCommoditiesTraded(context()->time(), {"spent_uox"});//GetInCommods());
  std::cout<<(context()->CommoditiesTraded(0)).size()<<"commodities traded in f\n";
  std::cout<<context()<<"commodity size for fac \n";
}

void Facility::EnterNotify() {
  Agent::EnterNotify();
    std::cout<<context()<<"commodity size for fac \n";
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
  //context()->UnregisterCommodityConsumer(in_commods_,this);

  Agent::Decommission();
}

bool Facility::CheckDecommissionCondition() {
  return NULL;
}

void Facility::Tock(){ // archetype developers need to invoke this method in tock 
  EventRequest();
}

void Facility::Tick(){
  //std::cout<<(context()->GetTest()).size()<<"\n\n";
  std::cout<<context()->GetStringTest()<<"getting string test \n\n\n";
  std::cout<< GetInCommods().size()<<"in fac tick the incommods in trader is \n\n\n";
  SetTraded(false); //archetype developers need to invoke this method in tick
}

void Facility::EventRequest(){
  // schedule_helper_.FixIncSchedule(); //FixIncSchedule schedules like cyclus 1.6v 
  // for(int i: schedule_helper_.EventTime()){ //probably needed in future... 
  //   selftimes_.insert(i);
  // }
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
