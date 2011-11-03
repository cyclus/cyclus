/// InstModel.cpp
// Implements the InstModel class

#include "InstModel.h"
#include "FacilityModel.h"
#include "Logician.h"
#include "InputXML.h"
#include "CycException.h"
#include "Logger.h"

#include <iostream>
#include <sstream>
#include <string>


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void InstModel::init(xmlNodePtr cur)
{
  Model::init(cur);
  LOG(LEV_DEBUG3) << "InstModel _-" << this->getName() << "-_ has finished Model's init " \
		  << "and has entered its own init function.";
  /** 
   *  Specific initialization for InstModels
   */
  
  /// determine the parent from the XML input
  string region_name = XMLinput->get_xpath_content(cur,"../name");
  Model* theParent = LI->getModelByName(region_name, REGION);
  this->setParent(theParent);
  LOG(LEV_DEBUG3) << "Parent " << theParent->getName() << " has added child " << this->getName();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void InstModel::copy(InstModel* src)
{
  // Copy parent classes
  Model::copy(src);
  Communicator::copy(src);

  /** 
   *  Specific initialization for InstModels
   */
  Model* parent = src->parent();
  parent->addChild(this);
  LI->addModel(this, INST);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void InstModel::print()
{
  Model::print();
}


/* --------------------
 * all COMMUNICATOR classes have these members
 * --------------------
 */
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void InstModel::receiveMessage(Message* msg){
  // Default institutions aren't insterested in fooling with messages.
  // Just pass them along. 
  // If it's going up, send it to the region.
  // If it's going down, send it to the facility.
  msg->setNextDest(parent());
  msg->sendOn();
}

void InstModel::handlePreHistory(){
  // tell all of the institution models to handle the tick
  for(vector<Model*>::iterator fac=children_.begin();
      fac != children_.end();
      fac++){
    //    LOG(LEV_DEBUG2) << "Inst " << ID << " is sending handleTick to facility " << (dynamic_cast<FacilityModel*>(*fac))->getFacName();
    (dynamic_cast<FacilityModel*>(*fac))->handlePreHistory();
  }
}

void InstModel::handleTick(int time){
  // tell all of the institution models to handle the tick
  for(vector<Model*>::iterator fac=children_.begin();
      fac != children_.end();
      fac++){
    //    LOG(LEV_DEBUG2) << "Inst " << ID << " is sending handleTick to facility " << (dynamic_cast<FacilityModel*>(*fac))->getFacName();
    (dynamic_cast<FacilityModel*>(*fac))->handleTick(time);
  }
}

void InstModel::handleTock(int time){
  // tell all of the institution models to handle the tick
  for(vector<Model*>::iterator fac=children_.begin();
      fac != children_.end();
      fac++){
    //    LOG(LEV_DEBUG2) << "Inst " << ID << " is sending handleTock to facility " << (dynamic_cast<FacilityModel*>(*fac))->getFacName();
    (dynamic_cast<FacilityModel*>(*fac))->handleTock(time);
  }
}

/* --------------------
 * all INSTMODEL classes have these members
 * --------------------
 */

bool InstModel::pleaseBuild(Model* fac){
  // by defualt
  std::stringstream ss;
  ss << this->getSN();
  throw CycOverrideException("Institution " + ss.str()
		     + " does not have a definied facility-building fuction.");
  return false;
}

double InstModel::getPowerCapacity(){
  // queries each facility for their power capacity
  double capacity = 0.0;
  for(vector<Model*>::iterator fac=children_.begin();
      fac != children_.end();
      fac++){
    capacity += (dynamic_cast<FacilityModel*>(*fac))->getPowerCapacity();
  }
  return capacity;
}
