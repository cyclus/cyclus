// StubComponent.cpp
#include <iostream>
#include <fstream>
#include <vector>
#include <time.h>

#include "GenException.h"
#include "InputXML.h"
#include "StubComponent.h"

using namespace std;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void StubComponent::init(xmlNodePtr cur){
  Component::init(cur);
  // move the xml pointer to the current model
  cur = XMLinput->get_xpath_element(cur,"model/StubComponent");
  // for now, just say you've done it... 
  cout << "The StubComponent Class init(cur) function has been called"<< endl;;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void StubComponent::copy(StubComponent* src){
  Component::copy(src);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void StubComponent::print(){
    cout << "StubComponent: " << this->getName();
    cout << "Contains Components:" << endl;
    for(int i=0; i<this->getDaughters().size() ; i++){
      cout << this->getDaughters()[i]->getName();
    }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void StubComponent::absorb(Material* matToAdd)
{
  // Get the given StubComponent's contaminant material.
  // add the material to it with the material absorb function.
  // each component should override this function
  cout << "StubComponent: " << this->getName() << endl;
  cout << "is absorbing material: " << endl;
  matToAdd->print();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void StubComponent::extract(Material* matToRem)
{
  // Get the given StubComponent's contaminant material.
  // add the material to it with the material extract function.
  // each component should override this function
  cout << "StubComponent: " << this->getName() << endl;
  cout << "is extracting material: " << endl;
  matToRem->print() ;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
Component* StubComponent::load(ComponentType type, Component* to_load)
{
  Component::load(type, to_load);
  return this;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool StubComponent::isFull() {
  return (daughter_components_.size()>=1) ? true : false ;
}

