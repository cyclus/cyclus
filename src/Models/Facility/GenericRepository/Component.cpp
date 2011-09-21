// Component.cpp
#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

#include "Component.h"
#include "GenException.h"
#include "Logician.h"

// Static variables to be initialized.
int Component::nextID_ = 0;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
Component::Component(): temperature_(0), inner_radius_(0), outer_radius_(0)
{
  name_ = "";
  ID_=nextID_++;
  //BI->registerVolChange(this);
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
Component::Component(xmlNodePtr cur)
{
  ID_=nextID_++;
  
  name_ = XMLinput->get_xpath_content(cur,"name");

  string vol_type_ = XMLinput->get_xpath_content(cur,"basis");

  vol_comp_hist_ = CompHistory() ;
  //BI->registerVolChange(this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
Component::Component(string name, Temp temp, Radius inner, Radius outer, 
    ConcMap concs, Solid* mat, Fluid* liq, ComponentType type)  
{
  
  ID_=nextID_++;
  name_ = name;
  temperature_ = temp;
  inner_radius_ = inner;
  outer_radius_ = outer;
  matrix_ = mat;
  liquid_ = liq;
  concentrations_ = concs;
  type_ = type;

  vol_comp_hist_ = CompHistory() ;
  //BI->registerVolChange(this);
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

