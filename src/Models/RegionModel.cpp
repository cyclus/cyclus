// RegionModel.cpp
// Implements the RegionModel class

#include <string>
#include <iostream>

#include "RegionModel.h"

#include "InstModel.h"
#include "CycException.h"
#include "InputXML.h"
#include "Timer.h"
#include "Logger.h"

using namespace std;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
void RegionModel::init() {
  // region models do not currently follow the template/not template
  // paradigm of insts and facs, so log this as its own parent
  this->setParent(this);
  // register to receive time-step notifications
  TI->registerTickListener(this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
void RegionModel::initAllowedFacilities(xmlNodePtr cur) {   
  xmlNodeSetPtr fac_nodes = XMLinput->get_xpath_elements(cur,"allowedfacility");
  string fac_name;
  Model* new_fac;
  for (int i=0;i<fac_nodes->nodeNr;i++){
    fac_name = (const char*)fac_nodes->nodeTab[i]->children->content;
    new_fac = Model::getTemplateByName(fac_name);
    allowedFacilities_.insert(new_fac);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void RegionModel::initChildren(xmlNodePtr cur) {   
  string inst_name;
  Model* inst;
  xmlNodeSetPtr inst_nodes = XMLinput->get_xpath_elements(cur,"institution");
  for (int i=0;i<inst_nodes->nodeNr;i++){
    inst_name = (const char*)XMLinput->get_xpath_content(inst_nodes->nodeTab[i],"name");
    inst = Model::getTemplateByName(inst_name);
    inst->setParent(this);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void RegionModel::init(xmlNodePtr cur) { 
  Model::init(cur); // name_ and model_impl_
  RegionModel::initAllowedFacilities(cur); // allowedFacilities_
  RegionModel::init(); // parent_ and tick listener, model 'born'
  RegionModel::initChildren(cur); // children->setParent, requires init()
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void RegionModel::copy(RegionModel* src) {
  Model::copy(src);
  Communicator::copy(src);

  /** 
   *  Specific initialization for RegionModels
   */

  allowedFacilities_ = src->allowedFacilities_;
  
  // don't copy institutions!

}
  
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
void RegionModel::print() {

  Model::print();

  LOG(LEV_DEBUG2, "none!") << "allows facilities " ;

  for(set<Model*>::iterator fac=allowedFacilities_.begin();
      fac != allowedFacilities_.end();
      fac++){
    LOG(LEV_DEBUG2, "none!") << (fac == allowedFacilities_.begin() ? "{" : ", " )
        << (*fac)->name();
  }
  
  LOG(LEV_DEBUG2, "none!") << "} and has the following institutions:";
  
  for(vector<Model*>::iterator inst=children_.begin();
      inst != children_.end();
      inst++){
    (*inst)->print();
  }
}

/* --------------------
 * all COMMUNICATOR classes have these members
 * --------------------
 */
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
void RegionModel::receiveMessage(msg_ptr msg){
  msg->setNextDest(msg->market());
  msg->sendOn();
}

void RegionModel::handlePreHistory(){
  // tell all of the institution models to handle the tick
  for(vector<Model*>::iterator inst=children_.begin();
      inst != children_.end();
      inst++){
    (dynamic_cast<InstModel*>(*inst))->handlePreHistory();
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
void RegionModel::handleTick(int time){
  // tell all of the institution models to handle the tick
  for(vector<Model*>::iterator inst=children_.begin();
      inst != children_.end();
      inst++){
    (dynamic_cast<InstModel*>(*inst))->handleTick(time);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
void RegionModel::handleTock(int time){
  // tell all of the institution models to handle the tick
  for(vector<Model*>::iterator inst=children_.begin();
      inst != children_.end();
      inst++){
    (dynamic_cast<InstModel*>(*inst))->handleTock(time);
  }
  
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
void RegionModel::handleDailyTasks(int time, int day){
  // tell all of the institution models to handle the tick
  for(vector<Model*>::iterator inst=children_.begin();
      inst != children_.end();
      inst++){
    (dynamic_cast<InstModel*>(*inst))->handleDailyTasks(time,day);
  }
}
