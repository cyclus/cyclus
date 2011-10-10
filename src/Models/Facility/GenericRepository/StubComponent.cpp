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
  // move the xml pointer to the current model
  cur = XMLinput->get_xpath_element(cur,"model/StubComponent");
  // for now, just say you've done it... 
  cout << "The StubComponent Class init(cur) function has been called"<< endl;;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void StubComponent::copy(StubComponent* src){
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void StubComponent::print(){
    cout << "StubComponent: " << this->getName() << endl;
    cout << "Contains Materials:" << endl;
    for(int i=0; i<this->getWastes().size() ; i++){
      cout << this->getWastes()[i];
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
  // add the material to it with the material absorb function.
  // each component should override this function
  cout << "StubComponent: " << this->getName() << endl;
  cout << "is extracting material: " << endl;
  matToRem->print() ;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool StubComponent::isFull() {
  // for now, return true and false at random
  return time(NULL) % 2;
}

