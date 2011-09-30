// FacilityModel.cpp
// Implements the FacilityModel class

#include "FacilityModel.h"

#include "Logician.h"
#include "BookKeeper.h"
#include "InputXML.h"

#include <iostream>
#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>

using namespace std;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void FacilityModel::init(xmlNodePtr cur) {
  Model::init(cur);

  // Specific initialization for FacilityModels
  xmlNodeSetPtr nodes = XMLinput->get_xpath_elements(cur, "/simulation/region/institution");
  
  for (int i=0;i<nodes->nodeNr;i++){
    inst_name_ = XMLinput->get_xpath_content(nodes->nodeTab[i], "name");
    setMemberVar("inst_name_",&inst_name_); 
  }
} 

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void FacilityModel::init(map<string,void*> member_var_map)
{
  Model::init(member_var_map);

  // Specific initialization for FacilityModels
  inst_name_ = getMapVar<string>("inst_name_",member_var_map);
  this->setInstName(inst_name_);
  cout << "Facility " << getSN() << " has just set its inst to " << inst_name_ << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void FacilityModel::copy(FacilityModel* src)
{ 
  Model::copy(src); 
  Communicator::copy(src); 

  // don't copy fac_jname to new instance
  fac_name_ = "";


  LI->addModel(this, FACILITY);
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
InstModel* FacilityModel::getFacInst() {
  return dynamic_cast<InstModel*>(LI->getModelByName(inst_name_, INST));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void FacilityModel::sendMaterial(Message* msg, vector<Material*> manifest) {
  // register this transaction with the bookkeper
  BI->registerTrans(msg, manifest);
  // send the material by calling the receiver's receiveMaterial function
  int recvID = msg->getRequesterID();
  dynamic_cast<FacilityModel*>(LI->getModelByID(recvID, FACILITY))->receiveMaterial(msg->getTrans(), manifest);
  std::cout << "Material sent from " << getSN() << " to " << recvID << "." << std::endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void FacilityModel::handlePreHistory() {
  // facilities should override this method, unless they're very naiive.
  // this function allows the facility to set up the simulation before it begins.
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void FacilityModel::handleTick(int time){
  // facilities should override this method, unless they're very naiive.
  // generally, a facility's handleTick() behavior should be to 
  // offer an amount of its output material equal to its capacity 
  // and to request an amount of raw material also equal to its capacity.
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void FacilityModel::handleTock(int time){
  // facilities should override this method, unless they're very naiive.
  // generally, a faility's handleTock() behavior should be to 
  // process any orders passed down from the market, 
  // send the appropriate materials, 
  // receive any materials the market has found a source for, 
  // and record all material transfers.
}

