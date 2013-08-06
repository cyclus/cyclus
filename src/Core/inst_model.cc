/// Instmodel.cc
// Implements the InstModel class

#include <iostream>
#include <sstream>
#include <string>

#include "inst_model.h"

#include "logger.h"
#include "timer.h"
#include "error.h"
#include "facility_model.h"
#include "query_engine.h"

namespace cyclus {

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
InstModel::InstModel() {
  SetModelType("Inst");
  prototypes_ = PrototypeSet();
  initial_build_order_ = std::map<Prototype*, int>();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void InstModel::InitCoreMembers(QueryEngine* qe) {
  Model::InitCoreMembers(qe);

  std::string name, query;
  int nEntries;
  Prototype* prototype;  
  
  // populate prototypes_
  query = "availableprototype";
  nEntries = qe->NElementsMatchingQuery(query);
  if (nEntries > 0) {
    // populate prototypes_
    for (int i=0;i<nEntries;i++){
      name = qe->GetElementContent(query,i);
      prototype = Prototype::GetRegisteredPrototype(name);
      AddAvailablePrototype(prototype);
    }
  } 

  query = "initialfacilitylist";
  nEntries = qe->NElementsMatchingQuery(query);
  // populate initial_build_order_
  if (nEntries > 0) {
    QueryEngine* list = qe->QueryElement(query);
    int numInitFacs = list->NElementsMatchingQuery("entry");
    for (int i=0;i<numInitFacs;i++){
      QueryEngine* entry = list->QueryElement("entry",i);
      AddPrototypeToInitialBuild(entry);
    }
  }

}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void InstModel::AddAvailablePrototype(Prototype* prototype) {
  prototypes_.insert(prototype);
  RegisterAvailablePrototype(prototype);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void InstModel::RegisterAvailablePrototype(Prototype* prototype) {}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void InstModel::AddPrototypeToInitialBuild(QueryEngine* qe) {
  
  std::string name = qe->GetElementContent("prototype");
  int number = atoi(qe->GetElementContent("number").c_str());

  Prototype* p = Prototype::GetRegisteredPrototype(name);
  ThrowErrorIfPrototypeIsntAvailable(p);

  CLOG(LEV_DEBUG3) << "Institution: " << this->name() << " is adding "
                   << number << " prototypes of type " << name 
                   << " to its list of initial facilities to build.";

  initial_build_order_.insert(std::make_pair(p, number));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void InstModel::ThrowErrorIfPrototypeIsntAvailable(Prototype* p) {
  if (!IsAvailablePrototype(p)) {    
    std::stringstream err("");
    err << "Inst " << this->name() << " does not have " 
        << dynamic_cast<Model*>(p)->name() 
        << " as one of its available prototypes.";
    throw ValidationError(err.str());
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
std::string InstModel::str() {
  try {
    return Model::str() + " in region" + parent()->name();
  } catch (ValueError err) {
    return Model::str() + " with no region.";
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void InstModel::EnterSimulationAsCoreEntity() {
  // build initial prototypes
  std::map<Prototype*,int>::iterator it;
  for (it = initial_build_order_.begin(); 
       it != initial_build_order_.end(); it ++) {
    
    // for each prototype
    Prototype* p = it->first;
    int number = it->second;

    for (int i = 0; i < number; i++) {
      // build as many as required
      Build(p);
    }
  }
}

/* --------------------
 * all COMMUNICATOR classes have these members
 * --------------------
 */
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void InstModel::ReceiveMessage(msg_ptr msg){
  // Default institutions aren't insterested in fooling with messages.
  // Just pass them along. 
  // If it's going up, send it to the region.
  // If it's going down, send it to the facility.
  msg->SendOn();
}

void InstModel::HandleTick(int time) {
  // tell all of the institution's child models to handle the tick
  int currsize = children_.size();
  int i = 0;
  while (i < children_.size()) {
    Model* m = children_.at(i);
    dynamic_cast<FacilityModel*>(m)->HandleTick(time);

    // increment not needed if a facility deleted itself
    if (children_.size() == currsize) {
      i++;
    }
    currsize = children_.size();
  }
}

void InstModel::HandleTock(int time) {
  // tell all of the institution's child models to handle the tock
  std::vector<FacilityModel*> children_to_decomm;

  for (int i = 0; i < children_.size(); i++) {
    FacilityModel* child = dynamic_cast<FacilityModel*>(children_.at(i));
    child->HandleTock(time);
    
    if ( child->LifetimeReached(time) ) {
      CLOG(LEV_INFO3) << child->name() << " has reached the end of its lifetime";
      if (child->CheckDecommissionCondition()) {
        children_to_decomm.push_back(child);
      }
    }
  }
  
  while(!children_to_decomm.empty()) {
    FacilityModel* child = children_to_decomm.back();
    children_to_decomm.pop_back();
    RegisterCloneAsDecommissioned(dynamic_cast<Prototype*>(child));
    child->Decommission();
  }
}

void InstModel::HandleDailyTasks(int time, int day){
  // tell all of the institution models to handle the tick
  for(std::vector<Model*>::iterator fac=children_.begin();
      fac != children_.end();
      fac++){
    dynamic_cast<FacilityModel*>(*fac)->HandleDailyTasks(time,day);
  }
}

/* --------------------
 * all INSTMODEL classes have these members
 * --------------------
 */
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void InstModel::Build(Prototype* prototype) {
  ThrowErrorIfPrototypeIsntAvailable(prototype);
  Prototype* clone = prototype->clone();
  dynamic_cast<Model*>(clone)->EnterSimulation(this);
  RegisterCloneAsBuilt(clone);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void InstModel::RegisterCloneAsBuilt(Prototype* clone) {}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void InstModel::RegisterCloneAsDecommissioned(Prototype* clone) {}

} // namespace cyclus
