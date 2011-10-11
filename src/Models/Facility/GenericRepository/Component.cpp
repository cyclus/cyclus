// Component.cpp
#include <iostream>
#include <fstream>
#include <vector>
#include <time.h>

#include "Component.h"
#include "GenException.h"
#include "InputXML.h"

using namespace std;

// Static variables to be initialized.
int Component::nextID_ = 0;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
Component::Component(): temperature_(0), inner_radius_(0), outer_radius_(0),
  temperature_lim_(373), toxicity_lim_(2)
{
  name_ = "";
  ID_=nextID_++;

  comp_hist_ = CompHistory();
  mass_hist_ = MassHistory();
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
Component::Component(string name, Temp temp, Temp temperature_lim, Tox toxicity_lim,
    Radius inner, Radius outer, ComponentType type)  
{
  
  ID_=nextID_++;
  name_ = name;
  temperature_ = temp;
  inner_radius_ = inner;
  outer_radius_ = outer;
  type_ = type;
  temperature_lim_ = temperature_lim ;
  toxicity_lim_ = toxicity_lim ;

  comp_hist_ = CompHistory();
  mass_hist_ = MassHistory();

}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Component::init(xmlNodePtr cur){
  ID_=nextID_++;
  
  name_ = XMLinput->get_xpath_content(cur,"name");

  comp_hist_ = CompHistory();
  mass_hist_ = MassHistory();
  // for now, just say you've done it... 
  cout << "The Component Class init(cur) function has been called"<< endl;;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Component::copy(Component* src){

  name_ = src->name_;
  temperature_ = src->temperature_;
  inner_radius_ = src->inner_radius_;
  outer_radius_ = src->outer_radius_;
  type_ = src->type_;
  temperature_lim_ = src->temperature_lim_ ;
  toxicity_lim_ = src->toxicity_lim_ ;

  comp_hist_ = CompHistory();
  mass_hist_ = MassHistory();
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
//void Component::absorb(Material* matToAdd)
//{
//  // Get the given Component's contaminant material.
//  // add the material to it with the material absorb function.
//  // each component should override this function
//  string err_msg = "Component Model : ";
//  err_msg += this->getName();
//  err_msg += " did not override absorb function.\n" ; 
//  throw GenException(err_msg);
//}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Component::extract(Material* matToRem)
{
  // Get the given Component's contaminant material.
  // subtract the material from it with the material extract function.
  // each component should override this function
  string err_msg = "Component Model : ";
  err_msg += this->getName();
  err_msg += " did not override extract function.\n"; 
  throw GenException(err_msg);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Component* Component::load(ComponentType type, Component* to_load) {
  Component* toRet = this;
  this->daughter_components_.push_back(to_load);
  return this;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool Component::isFull() {
  // each component should override this function
  string err_msg = "Component Model : ";
  err_msg += this->getName();
  err_msg += " did not override the isFull function.\n"; 
  throw GenException(err_msg);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ComponentType Component::getComponentType(string type_name) {
  ComponentType toRet;
  string component_type_names[] = {"ENV", "FF", "NF", "BUFFER", "WP", "WF"};
  for(int type = 0; type < LAST_TYPE; type++){
    if(component_type_names[type] == type_name){
      toRet = (ComponentType)type;
    } else {
      string err_msg ="'";
      err_msg += type_name;
      err_msg += "' does not name a valid ComponentType.\n";
      throw GenException(err_msg);
    }
  }
  return toRet;
}
