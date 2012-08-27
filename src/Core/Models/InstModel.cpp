/// InstModel.cpp
// Implements the InstModel class

#include <iostream>
#include <sstream>
#include <string>

#include "InstModel.h"

#include "Logger.h"
#include "Timer.h"
#include "CycException.h"
#include "FacilityModel.h"
#include "QueryEngine.h"

using namespace std;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
InstModel::InstModel() {
  setModelType("Inst");
  prototypes_ = PrototypeSet();
  initial_build_order_ = map<Prototype*,int>();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void InstModel::initCoreMembers(QueryEngine* qe) {
  Model::initCoreMembers(qe);

  string name;
  Prototype* prototype;  
  
  // populate prototypes_
  try {
    int numAvailProtos = qe->numElementsMatchingQuery("availableprototype");
    
    // populate prototypes_
    for (int i=0;i<numAvailProtos;i++){
      name = qe->getElementContent("availableprototype",i);
      prototype = Prototype::getRegisteredPrototype(name);
      prototypes_.insert(prototype);
    }
  } catch (CycNullQueryException) {}; // no prototypes available

  // populate initial_build_order_
  try {
    int numInitFacs = qe->numElementsMatchingQuery("initialfacilitylist");
    for (int i=0;i<numInitFacs;i++){
      QueryEngine* qe_child = qe->queryElement("initialfacilitylist",1);
      addPrototypeToInitialBuild(qe_child);
    }
  } catch (CycNullQueryException) {}; // no initial builds

}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void InstModel::addPrototypeToInitialBuild(QueryEngine* qe) {
  
  string name = qe->getElementContent("prototype");
  int number = atoi(qe->getElementContent("number").c_str());

  Prototype* p = Prototype::getRegisteredPrototype(name);
  throwErrorIfPrototypeIsntAvailable(p);
  initial_build_order_.insert(make_pair(p,number));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void InstModel::throwErrorIfPrototypeIsntAvailable(Prototype* p) {
  if (!isAvailablePrototype(p)) {    
    stringstream err("");
    err << "Inst " << this->name() << " does not have " 
        << dynamic_cast<Model*>(p)->name() 
        << " as one of its available prototypes.";
    throw CycOverrideException(err.str());
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
std::string InstModel::str() {
  try {
    return Model::str() + " in region" + parent()->name();
  } catch (CycIndexException err) {
    return Model::str() + " with no region.";
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void InstModel::enterSimulation(Model* parent) {
  Model::enterSimulation(parent);

  // build initial prototypes
  map<Prototype*,int>::iterator it;
  for (it = initial_build_order_.begin(); 
       it != initial_build_order_.end(); it ++) {
    
    // for each prototype
    Prototype* p = it->first;
    int number = it->second;

    for (int i = 0; i < number; i++) {
      // build as many as required
      build(p);
    }

  }
}

/* --------------------
 * all COMMUNICATOR classes have these members
 * --------------------
 */
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void InstModel::receiveMessage(msg_ptr msg){
  // Default institutions aren't insterested in fooling with messages.
  // Just pass them along. 
  // If it's going up, send it to the region.
  // If it's going down, send it to the facility.
  msg->sendOn();
}

void InstModel::handleTick(int time) {
  // tell all of the institution's child models to handle the tick
  int currsize = children_.size();
  int i = 0;
  while (i < children_.size()) {
    Model* m = children_.at(i);
    dynamic_cast<FacilityModel*>(m)->handleTick(time);

    // increment not needed if a facility deleted itself
    if (children_.size() == currsize) {
      i++;
    }
    currsize = children_.size();
  }
}

void InstModel::handleTock(int time) {
  // tell all of the institution's child models to handle the tock
  int currsize = children_.size();
  int i = 0;
  while (i < children_.size()) {
    Model* m = children_.at(i);
    dynamic_cast<FacilityModel*>(m)->handleTock(time);

    // increment not needed if a facility deleted itself
    if (children_.size() == currsize) {
      i++;
    }
    currsize = children_.size();
  }
}

void InstModel::handleDailyTasks(int time, int day){
  // tell all of the institution models to handle the tick
  for(vector<Model*>::iterator fac=children_.begin();
      fac != children_.end();
      fac++){
    dynamic_cast<FacilityModel*>(*fac)->handleDailyTasks(time,day);
  }
}

/* --------------------
 * all INSTMODEL classes have these members
 * --------------------
 */
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void InstModel::build(Prototype* prototype) {
  Prototype* clone = prototype->clone();
  dynamic_cast<Model*>(clone)->enterSimulation(this);
}

