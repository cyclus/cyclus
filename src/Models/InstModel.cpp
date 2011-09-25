/// InstModel.cpp
// Implements the InstModel class

#include "InstModel.h"
#include <sstream>

#include "Logician.h"
#include "InputXML.h"
#include "GenException.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void InstModel::init(xmlNodePtr cur)
{
  Model::init(cur);

  /** 
   *  Specific initialization for InstModels
   */
  
  /// determine the parent from the XML input
  string region_name = XMLinput->get_xpath_content(cur,"../name");
  region_ = LI->getModelByName(region_name, REGION);
  this->setRegion(region_);
  cout << "Inst " << getSN() << " has set its region to be " << region_name << endl;
  
  dynamic_cast<RegionModel*>(region_)->addInstitution(this);

}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void InstModel::copy(InstModel* src)
{
  Model::copy(src);
  Communicator::copy(src);

  /** 
   *  Specific initialization for InstModels
   */
  
  region_ = src->region_;
  dynamic_cast<RegionModel*>(region_)->addInstitution(this);
  LI->addModel(this, INST);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void InstModel::print()
{
  Model::print();

  cout << "in region " << region_->getName();
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
  MessageDir dir = msg->getDir();
  if (dir == UP_MSG){
    Communicator* nextRecipient = msg->getReg();
    nextRecipient->receiveMessage(msg);
  }
  else if (dir == DOWN_MSG){
    Communicator* nextRecipient = msg->getFac();
    nextRecipient->receiveMessage(msg);
  }
}

void InstModel::handlePreHistory(){
  // tell all of the institution models to handle the tick
  for(vector<Model*>::iterator fac=facilities_.begin();
      fac != facilities_.end();
      fac++){
    //    cout << "Inst " << ID << " is sending handleTick to facility " << (dynamic_cast<FacilityModel*>(*fac))->getFacName() << endl;
    (dynamic_cast<FacilityModel*>(*fac))->handlePreHistory();
  }
}

void InstModel::handleTick(int time){
  // tell all of the institution models to handle the tick
  for(vector<Model*>::iterator fac=facilities_.begin();
      fac != facilities_.end();
      fac++){
    //    cout << "Inst " << ID << " is sending handleTick to facility " << (dynamic_cast<FacilityModel*>(*fac))->getFacName() << endl;
    (dynamic_cast<FacilityModel*>(*fac))->handleTick(time);
  }
}

void InstModel::handleTock(int time){
  // tell all of the institution models to handle the tick
  for(vector<Model*>::iterator fac=facilities_.begin();
      fac != facilities_.end();
      fac++){
    //    cout << "Inst " << ID << " is sending handleTock to facility " << (dynamic_cast<FacilityModel*>(*fac))->getFacName() << endl;
    (dynamic_cast<FacilityModel*>(*fac))->handleTock(time);
  }
}

/* --------------------
 * all INSTMODEL classes have these members
 * --------------------
 */

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
bool InstModel::pleaseBuild(Model* fac){
  // by defualt
  std::stringstream ss;
  ss << this->getSN();
  throw GenException("Institution " + ss.str()
		     + " does not have a definied facility-building fuction.");
  return false;
}

double InstModel::getPowerCapacity(){
  // queries each facility for their power capacity
  double capacity = 0.0;
  for(vector<Model*>::iterator fac=facilities_.begin();
      fac != facilities_.end();
      fac++){
    capacity += (dynamic_cast<FacilityModel*>(*fac))->getPowerCapacity();
  }
  return capacity;
}
