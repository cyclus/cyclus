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

  vol_comp_hist_ = CompHistory();
  vol_mass_hist_ = MassHistory();
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
Component::Component(xmlNodePtr cur)
{
  ID_=nextID_++;
  
  name_ = XMLinput->get_xpath_content(cur,"name");

  string vol_type_ = XMLinput->get_xpath_content(cur,"basis");

  vol_comp_hist_ = CompHistory();
  vol_mass_hist_ = MassHistory();
}

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

  vol_comp_hist_ = CompHistory();
  vol_mass_hist_ = MassHistory();

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
void Component::absorb(Material* matToAdd)
{
  // Get the given Component's contaminant material.
  // add the material to it with the material absorb function.
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Component::extract(Material* matToRem)
{
  // Get the given Component's contaminant material.
  // subtract the material from it with the material extract function.
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool Component::isFull() {
  // for now, return true and false at random
  return time(NULL) % 2;
}
