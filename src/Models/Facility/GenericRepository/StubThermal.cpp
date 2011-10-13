// StubThermal.cpp
#include <iostream>
#include <fstream>
#include <vector>
#include <time.h>

#include "GenException.h"
#include "InputXML.h"
#include "StubThermal.h"

using namespace std;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void StubThermal::init(xmlNodePtr cur){
  ThermalModel::init(cur);
  // move the xml pointer to the current model
  cur = XMLinput->get_xpath_element(cur,"model/StubThermal");
  // for now, just say you've done it... 
  cout << "The StubThermal Class init(cur) function has been called"<< endl;;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void StubThermal::copy(StubThermal* src){
  ThermalModel::copy(src);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void StubThermal::print(){
    cout << "StubThermal: '" << this->getName() << endl;
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
Power StubThermal::getAvailCapacity(Component* comp){
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
Temp StubThermal::getPeakTemp(BoundaryType type, Component* comp){
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
Temp StubThermal::getTemp(Component* comp){
}


