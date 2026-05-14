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
#include "toolkit/scheduling_function.h"

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
  //for agents WITHOUT the need for a checkdecom function, they can easily schedule decom at build (only reactor/separations use this right now)
  if (lifetime() >= 0 && CheckDecommissionCondition() == NULL) { 
    context()->SchedDecom(this, exit_time());
  }
  //all agents who have been build should want to immediately trade 
  InitialTrade(); 
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

void Facility::Tock(){ // archetype developers need to invoke this method in tock 
  EventRequest();
}

void Facility::Tick(){
  SetTraded(false); //archetype developers need to invoke this method in tick
}

std::set<int> Facility::GetSchedulingTime(){ 
  cyclus::toolkit::SchedulingFunctions sc(this); //not ideal.... this class instance will be made every tock... 
  sc.FixIncSchedule(); //FixIncSchedule schedules like cyclus 1.6v 
  std::set<int> EventTime = sc.EventTime();
  sc.clear();
  return EventTime;
}

void Facility::EventRequest(){
  for(int i: GetSchedulingTime()){
    context()->RegisterRequesters(i, this);
    selftimes_.insert(i);
  }
}

void Facility::InitialTrade(){
  if(context()->time() ==0){
    context()->RegisterRequesters(1,this); //if sim_tims (t=0) register for time =1
  }
  else if (context()->time()>0){ //if during sim_time >0 register for sim_time
    context()->RegisterRequesters(context()->time(),this);
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
