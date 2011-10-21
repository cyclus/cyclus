// Component.cpp
#include <iostream>
#include <fstream>
#include <vector>
#include <time.h>

#include "GenException.h"
#include "Component.h"
//#include "LLNLThermal.h"
//#include "LumpThermal.h"
//#include "SindaThermal.h"
#include "StubThermal.h"
//#include "LumpNuclide.h"
//#include "MixedCellNuclide.h"
#include "StubNuclide.h"
#include "InputXML.h"

using namespace std;

// Static variables to be initialized.
int Component::nextID_ = 0;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Component::Component(){
  name_ = "";
  inner_radius_ = 0;
  outer_radius_ = 0;
  temperature_ = 0;

  temperature_lim_ = 373;
  toxicity_lim_ = 10 ;

  thermal_model_ = NULL;
  nuclide_model_ = NULL;

  comp_hist_ = CompHistory();
  mass_hist_ = MassHistory();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Component::init(xmlNodePtr cur){
  ID_=nextID_++;
  
  name_ = XMLinput->get_xpath_content(cur,"name");

  thermal_model_ = getThermalModel(cur);
  nuclide_model_ = getNuclideModel(cur);

  comp_hist_ = CompHistory();
  mass_hist_ = MassHistory();
  // for now, just say you've done it... 
  cout << "The Component Class init(cur) function has been called."<< endl;;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Component::copy(Component* src){

  name_ = src->name_;
  inner_radius_ = src->inner_radius_;
  outer_radius_ = src->outer_radius_;

  type_ = src->type_;

  thermal_model_ = copyThermalModel(src->thermal_model_);
  nuclide_model_ = copyNuclideModel(src->nuclide_model_);

  temperature_lim_ = src->temperature_lim_ ;
  toxicity_lim_ = src->toxicity_lim_ ;

  comp_hist_ = CompHistory();
  mass_hist_ = MassHistory();

  temperature_ = src->temperature_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void Component::print(){
  cout << "Component: " << this->getName() << endl;
  cout << "Contains Materials:" << endl;
  for(int i=0; i<this->getWastes().size() ; i++){
    cout << wastes_[i];
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Component* Component::load(ComponentType type, Component* to_load) {
  Component* toRet = this;
  this->daughter_components_.push_back(to_load);
  to_load->parent_component_ = this;
  return this;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool Component::isFull() {
  return true; //TEMPORARY
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ComponentType Component::getComponentType(std::string type_name) {
  ComponentType toRet = LAST_EBS;
  string component_type_names[] = {"BUFFER", "ENV", "FF", "NF", "WF", "WP"};
  for(int type = 0; type < LAST_EBS; type++){
    if(component_type_names[type] == type_name){
      toRet = (ComponentType)type;
    } 
  }
  if (toRet == LAST_EBS){
    string err_msg ="'";
    err_msg += type_name;
    err_msg += "' does not name a valid ComponentType.\n";
    err_msg += "Options are:\n";
    for(int name=0; name < LAST_EBS; name++){
      err_msg += component_type_names[name];
      err_msg += "\n";
    }
    throw GenException(err_msg);
  }
  return toRet;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ThermalModelType Component::getThermalModelType(std::string type_name) {
  ThermalModelType toRet = LAST_THERMAL;
  string thermal_type_names[] = {"LLNLThermal","LumpThermal","SindaThermal","StubThermal"};
  for(int type = 0; type < LAST_THERMAL; type++){
    if(thermal_type_names[type] == type_name){
      toRet = (ThermalModelType)type;
    } 
  }
  if (toRet == LAST_THERMAL){
    string err_msg ="'";
    err_msg += type_name;
    err_msg += "' does not name a valid ThermalModelType.\n";
    err_msg += "Options are:\n";
    for(int name=0; name < LAST_THERMAL; name++){
      err_msg += thermal_type_names[name];
      err_msg += "\n";
    }     
    throw GenException(err_msg);
  }
  return toRet;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
NuclideModelType Component::getNuclideModelType(std::string type_name) {
  NuclideModelType toRet = LAST_NUCLIDE;
  string nuclide_type_names[] = {"LumpNuclide","MixedCellNuclide", "StubNuclide" };
  for(int type = 0; type < LAST_NUCLIDE; type++){
    if(nuclide_type_names[type] == type_name){
      toRet = (NuclideModelType)type;
    }
  }
  if (toRet == LAST_NUCLIDE){
    string err_msg ="'";
    err_msg += type_name;
    err_msg += "' does not name a valid NuclideModelType.\n";
    err_msg += "Options are:\n";
    for(int name=0; name < LAST_NUCLIDE; name++){
      err_msg += nuclide_type_names[name];
      err_msg += "\n";
    }
    throw GenException(err_msg);
  }
  return toRet;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
ThermalModel* Component::getThermalModel(xmlNodePtr cur){
  ThermalModel* toRet;
  string model_name = XMLinput->get_xpath_name(cur,"thermalmodel/*");
  
  switch(getThermalModelType(model_name))
  {
//    case LLNL_THERMAL:
//      toRet = new LLNLThermal(cur);
//      break;
//    case LUMP_THERMAL:
//      toRet = new LumpThermal(cur);
//      break;
//    case SINDA_THERMAL:
//      toRet = new SindaThermal(cur);
//      break;
    case STUB_THERMAL:
      toRet = new StubThermal(cur);
      break;
    default:
      throw GenException("Unknown thermal model enum value encountered."); 
  }
  return toRet;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
NuclideModel* Component::getNuclideModel(xmlNodePtr cur){
  NuclideModel* toRet;

  string model_name = XMLinput->get_xpath_name(cur,"nuclidemodel/*");

  switch(getNuclideModelType(model_name))
  {
//    case LUMP_NUCLIDE:
//      toRet = new LumpNuclide(cur);
//      break;
//    case MIXEDCELLL_NUCLIDE:
//      toRet = new LLNLNuclide(cur);
//      break;
    case STUB_NUCLIDE:
      toRet = new StubNuclide(cur);
      break;
    default:
      throw GenException("Unknown nuclide model enum value encountered."); 
  }
  return toRet;
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
ThermalModel* Component::copyThermalModel(ThermalModel* src){
  ThermalModel* toRet;
  switch( src->getThermalModelType() )
  {
//    case LLNL_THERMAL:
//      toRet = new LLNLThermal();
//      toRet->copy(src);
//      break;
//    case LUMP_THERMAL:
//      toRet = new LumpThermal();
//      toRet->copy(src);
//      break;
//    case SINDA_THERMAL:
//      toRet = new SindaThermal();
//      toRet->copy(src);
//      break;
    case STUB_THERMAL:
      toRet = new StubThermal();
      toRet->copy(src);
      break;
    default:
      throw GenException("Unknown thermal model enum value encountered when copying."); 
  }      
  return toRet;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
NuclideModel* Component::copyNuclideModel(NuclideModel* src){
  NuclideModel* toRet;
  switch(src->getNuclideModelType())
  {
//    case LUMP_NUCLIDE:
//      toRet = new LumpNuclide();
//      toRet->copy(src);
//      break;
//    case MIXEDCELL_NUCLIDE:
//      toRet = new MixedCellNuclide();
//      toRet->copy(src);
//      break;
    case STUB_NUCLIDE:
      toRet = new StubNuclide();
      toRet->copy(src);
      break;
    default:
      throw GenException("Unknown nuclide model enum value encountered when copying."); 
  }      
  return toRet;
}

