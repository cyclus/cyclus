// RegionModel.cpp
// Implements the RegionModel class

#include <string>
#include <iostream>

#include "RegionModel.h"

#include "Model.h"
#include "InstModel.h"
#include "CycException.h"
#include "Timer.h"
#include "Logger.h"
#include "Prototype.h"
#include "QueryEngine.h"

using namespace std;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
RegionModel::RegionModel() { 
  setModelType("Region");
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void RegionModel::initCoreMembers(QueryEngine* qe) { 
  Model::initCoreMembers(qe); // name_
  RegionModel::initAllowedFacilities(qe); // allowedFacilities_
  RegionModel::initInstitutionNames(qe); // inst_names_
  //RegionModel::addRegionAsRootNode(); // parent_ and tick listener, model 'born'
  //RegionModel::addChildrenToTree(qe); // children->setParent, requires initCoreMembers()
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void RegionModel::initAllowedFacilities(QueryEngine* qe) {   
  int num_allowed_fac = qe->numElementsMatchingQuery("allowedfacility");
  string fac_name;
  Model* new_fac;
  for (int i=0;i<num_allowed_fac;i++) {
    fac_name = qe->getElementContent("allowedfacility",i);
    new_fac = dynamic_cast<Model*>(Prototype::getRegisteredPrototype(fac_name));
    allowedFacilities_.insert(new_fac);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void RegionModel::initInstitutionNames(QueryEngine* qe) {
  int nInsts = qe->numElementsMatchingQuery("institution");
  string name;
  for (int i=0; i<nInsts; i++) {
    QueryEngine* inst_data = qe->queryElement("institution",i);
    name = inst_data->getElementContent("name");
    inst_names_.insert(name);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void RegionModel::enterSimulation(Model* parent) {
  if (parent != this) {
    string err = "Regions must be their own parent.";
    throw CycOverrideException(err);
  }
  addRegionAsRootNode();
  addChildrenToTree();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void RegionModel::addRegionAsRootNode() {
  Model::enterSimulation(this);
  TI->registerTickListener(this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void RegionModel::addChildrenToTree() {
  Model* inst;
  set<string>::iterator it;
  for (it = inst_names_.begin(); it != inst_names_.end(); it++){
    inst = Model::getModelByName((*it));
    inst->enterSimulation(this);
  }
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
  msg->sendOn();
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
