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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
RegionModel::RegionModel() { 
  setModelType("Region");
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void RegionModel::init(xmlNodePtr cur) { 
  Model::init(cur); // name_ and model_impl_
  RegionModel::initAllowedFacilities(cur); // allowedFacilities_
  RegionModel::initSimInteraction(this); // parent_ and tick listener, model 'born'
  RegionModel::initChildren(cur); // children->setParent, requires init()
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void RegionModel::initAllowedFacilities(xmlNodePtr cur) {   
  xmlNodeSetPtr fac_nodes = 
    XMLinput->get_xpath_elements(cur,"allowedfacility");
  string fac_name;
  Model* new_fac;
  for (int i=0;i<fac_nodes->nodeNr;i++){
    fac_name = (const char*)fac_nodes->nodeTab[i]->children->content;
    new_fac = Model::getTemplateByName(fac_name);
    allowedFacilities_.insert(new_fac);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void RegionModel::initSimInteraction(RegionModel* reg) {
  reg->setParent(reg);
  TI->registerTickListener(reg);
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
void RegionModel::copy(RegionModel* src) {
  Model::copy(src);
  Communicator::copy(src);
  allowedFacilities_ = src->allowedFacilities_;
  RegionModel::initSimInteraction(src);
}
  
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
std::string RegionModel::str() {
  std::string s = Model::str();

  s += "allows facs: ";
  for(set<Model*>::iterator fac=allowedFacilities_.begin();
      fac != allowedFacilities_.end();
      fac++){
    s += (*fac)->name() + ", ";
  }

  s += ". And has insts: ";
  for(vector<Model*>::iterator inst=children_.begin();
      inst != children_.end();
      inst++){
    s += (*inst)->name() + ", ";
  }
  return s;
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
  int currsize = children_.size();
  int i = 0;
  while (i < children_.size()) {
    Model* m = children_.at(i);
    dynamic_cast<InstModel*>(m)->handleTick(time);

    // increment not needed if a facility deleted itself
    if (children_.size() == currsize) {
      i++;
    }
    currsize = children_.size();
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
void RegionModel::handleTock(int time){
  int currsize = children_.size();
  int i = 0;
  while (i < children_.size()) {
    Model* m = children_.at(i);
    dynamic_cast<InstModel*>(m)->handleTock(time);

    // increment not needed if a facility deleted itself
    if (children_.size() == currsize) {
      i++;
    }
    currsize = children_.size();
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
