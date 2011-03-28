/// InstModel.cpp
// Implements the InstModel class

#include "InstModel.h"
#include <sstream>

/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

// Initialize the InstModel nextID to zero.
int InstModel::nextID = 0;

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
  region = LI->getRegionByName(region_name);
  this->setRegion(region);
  cout << "Inst " << ID << " has set its region to be " << region_name << endl;
  
  ((RegionModel*)region)->addInstitution(this);

}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void InstModel::copy(InstModel* src)
{
  Model::copy(src);
  Communicator::copy(src);

  /** 
   *  Specific initialization for InstModels
   */
  
  region = src->region;
  ((RegionModel*)region)->addInstitution(this);
  LI->addInst(this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void InstModel::print()
{
  Model::print();

  cout << "in region " << region->getName();
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
  if (dir == up){
    Communicator* nextRecipient = msg->getReg();
    nextRecipient->receiveMessage(msg);
  }
  else if (dir == down){
    Communicator* nextRecipient = msg->getFac();
    nextRecipient->receiveMessage(msg);
  }
}

void InstModel::handleTick(int time){
  // tell all of the institution models to handle the tick
  for(vector<Model*>::iterator fac=facilities.begin();
      fac != facilities.end();
      fac++){
    cout << "Inst " << ID << " is sending handleTick to facility " << ((FacilityModel*)(*fac))->getFacName() << endl;
    ((FacilityModel*)(*fac))->handleTick(time);
  }
}
void InstModel::handleTock(int time){
  // tell all of the institution models to handle the tick
  for(vector<Model*>::iterator fac=facilities.begin();
      fac != facilities.end();
      fac++){
    cout << "Inst " << ID << " is sending handleTock to facility " << ((FacilityModel*)(*fac))->getFacName() << endl;
    ((FacilityModel*)(*fac))->handleTock(time);
  }
}

/* --------------------
 * all INSTMODEL classes have these members
 * --------------------
 */

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void InstModel::pleaseBuild(Model* fac){
  // by defualt
  std::stringstream ss;
  ss << this->ID;
  throw GenException("Institution " + ss.str()
		     + " does not have a definied facility-building fuction.");
}
