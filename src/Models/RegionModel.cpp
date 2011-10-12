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
    allowedFacilities_.insert(new_fac);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
void RegionModel::copy(RegionModel* src) {
  Model::copy(src);
  Communicator::copy(src);

  /** 
   *  Specific initialization for RegionModels
   */

  allowedFacilities_ = src->allowedFacilities_;
  
  // don't copy institutions!
  LI->addModel(this, REGION);

}
  
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
void RegionModel::print() {

  Model::print();

  cout << "allows facilities " ;

  for(set<Model*>::iterator fac=allowedFacilities_.begin();
      fac != allowedFacilities_.end();
      fac++){
    cout << (fac == allowedFacilities_.begin() ? "{" : ", " )
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
  msg->setNextDest(msg->getMarket());
  msg->sendOn();
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
