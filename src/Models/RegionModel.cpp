// RegionModel.cpp
// Implements the RegionModel class

#include "RegionModel.h"
#include "InstModel.h"
#include "GenException.h"
#include "Logician.h"
#include "InputXML.h"
#include "Timer.h"

#include <string>
#include <iostream>

using namespace std;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
  /// Default constructor for RegionModel Class
  RegionModel::RegionModel() {
    setModelType("Region");
    commType_=REGION_COMM; 

    // register to receive time-step notifications
    TI->registerAgent(this);
  };

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
void RegionModel::init(xmlNodePtr cur)
{
 
  Model::init(cur);

  /** 
   *  Specific initialization for RegionModels
   */

  /// all regions require allowed facilities - possibly many
  xmlNodeSetPtr nodes = XMLinput->get_xpath_elements(cur,"allowedfacility");

  string fac_name;
  Model* new_fac;
  
  for (int i=0;i<nodes->nodeNr;i++){
    fac_name = (const char*)nodes->nodeTab[i]->children->content;
    new_fac = LI->getModelByName(fac_name, FACILITY);
    if (NULL == new_fac){
      throw GenException("Facility " + fac_name 
          + " is not defined in this simulation.");
    }
    allowed_facilities_.insert(new_fac);
  }
  setMemberVar("allowed_facilities_",&allowed_facilities_);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
void RegionModel::init(map<string, void*>member_var_map)
{
 
  Model::init(member_var_map);

  /** 
   *  Specific initialization for RegionModels
   */

  /// all regions require allowed facilities - possibly many
  allowed_facilities_ = getMapVar<set< Model*> >("allowed_facilities_", member_var_map);
  setMemberVar("allowed_facilities_",&allowed_facilities_);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
void RegionModel::copy(RegionModel* src) {
  Model::copy(src);
  Communicator::copy(src);

  /** 
   *  Specific initialization for RegionModels
   */

  allowed_facilities_ = src->allowed_facilities_;
  
  // don't copy institutions!
  LI->addModel(this, REGION);

}
  
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
void RegionModel::print() {

  Model::print();

  cout << "allows facilities " ;

  for(set<Model*>::iterator fac=allowed_facilities_.begin();
      fac != allowed_facilities_.end();
      fac++){
    cout << (fac == allowed_facilities_.begin() ? "{" : ", " )
        << (*fac)->getName();
  }
  
  cout << "} and has the following institutions:" << endl;
  
  for(vector<Model*>::iterator inst=institutions_.begin();
      inst != institutions_.end();
      inst++){
    (*inst)->print();
  }
}

/* --------------------
 * all COMMUNICATOR classes have these members
 * --------------------
 */
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
void RegionModel::receiveMessage(Message* msg){
  // Default regions aren't insterested in fooling with messages.
  // Just pass them along. 
  // If it's going up, send it to the market.
  // If it's going down, send it to the inst.
  MessageDir dir = msg->getDir();
  if (dir == UP_MSG){
    Communicator* nextRecipient = msg->getMkt();
    nextRecipient->receiveMessage(msg);
  }
  else if (dir == DOWN_MSG){
    Communicator* nextRecipient = msg->getInst();
    nextRecipient->receiveMessage(msg);
  }
}

void RegionModel::handlePreHistory(){
  // tell all of the institution models to handle the tick
  for(vector<Model*>::iterator inst=institutions_.begin();
      inst != institutions_.end();
      inst++){
    (dynamic_cast<InstModel*>(*inst))->handlePreHistory();
  }
}

void RegionModel::handleTick(int time){
  // tell all of the institution models to handle the tick
  for(vector<Model*>::iterator inst=institutions_.begin();
      inst != institutions_.end();
      inst++){
    (dynamic_cast<InstModel*>(*inst))->handleTick(time);
  }
}

void RegionModel::handleTock(int time){
  // tell all of the institution models to handle the tick
  for(vector<Model*>::iterator inst=institutions_.begin();
      inst != institutions_.end();
      inst++){
    (dynamic_cast<InstModel*>(*inst))->handleTock(time);
  }
}

/* --------------------
 * all REGIONMODEL classes have these members
 * --------------------
 */
