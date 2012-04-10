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
  init();
  setModelType("Inst");
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void InstModel::init() {
  prototypes_ = new PrototypeSet();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void InstModel::init(xmlNodePtr cur) {
  // non xml inits
  InstModel::init();
  // xml inits
  Model::init(cur);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void InstModel::copy(InstModel* src) {
  Model::copy(src);
  Communicator::copy(src);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void InstModel::print()
{
  Model::print();

  LOG(LEV_DEBUG2, "none!") << "in region " << parent()->name();
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
  msg->setNextDest( (dynamic_cast<Communicator*>( parent() )) );
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

void InstModel::handleTick(int time){
  // tell all of the institution models to handle the tick
  for(vector<Model*>::iterator fac=children_.begin();
      fac != children_.end();
      fac++){
    (dynamic_cast<FacilityModel*>(*fac))->handleTick(time);
  }
}

void InstModel::handleTock(int time){
  // tell all of the institution models to handle the tick
  for(vector<Model*>::iterator fac=children_.begin();
      fac != children_.end();
      fac++){
    (dynamic_cast<FacilityModel*>(*fac))->handleTock(time);
  }
}

void InstModel::handleDailyTasks(int time, int day){
  // tell all of the institution models to handle the tick
  for(vector<Model*>::iterator fac=children_.begin();
      fac != children_.end();
      fac++){
    (dynamic_cast<FacilityModel*>(*fac))->handleDailyTasks(time,day);
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

