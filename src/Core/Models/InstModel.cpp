/// InstModel.cpp
// Implements the InstModel class

#include <iostream>
#include <sstream>
#include <string>

#include "InstModel.h"

#include "Logger.h"
#include "Timer.h"
#include "InputXML.h"
#include "CycException.h"
#include "FacilityModel.h"

using namespace std;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
InstModel::InstModel() {
  setModelType("Inst");
  prototypes_ = new PrototypeSet();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void InstModel::init(xmlNodePtr cur) {
  Model::init(cur);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void InstModel::copy(InstModel* src) {
  Model::copy(src);
  Communicator::copy(src);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
std::string InstModel::str() {
  try {
    return Model::str() + " in region" + parent()->name();
  } catch (CycIndexException err) {
    return Model::str() + " with no region.";
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

void InstModel::handlePreHistory(){
  // tell all of the institution models to handle the tick
  for(vector<Model*>::iterator fac=children_.begin();
      fac != children_.end();
      fac++){
    (dynamic_cast<FacilityModel*>(*fac))->handlePreHistory();
  }
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
void InstModel::addPrototype(Model* prototype) {
  if ( !isAvailablePrototype(prototype) ) {
    prototypes_->insert(prototype);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void InstModel::build(Model* prototype, Model* requester) {
  // by default
  stringstream err("");
  err << "Institution " << this->name() << " does not have a definied " 
      << "facility-building fuction.";
  throw CycOverrideException(err.str());
}

double InstModel::powerCapacity(){
  // queries each facility for their power capacity
  double capacity = 0.0;
  for(vector<Model*>::iterator fac=children_.begin();
      fac != children_.end();
      fac++){
    capacity += (dynamic_cast<FacilityModel*>(*fac))->powerCapacity();
  }
  return capacity;
}

