// StubThermal.cpp
#include <iostream>
#include "Logger.h"
#include <fstream>
#include <vector>
#include <time.h>

#include "CycException.h"
#include "InputXML.h"
#include "StubThermal.h"

using namespace std;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void StubThermal::init(xmlNodePtr cur){
  // move the xml pointer to the current model
  cur = XMLinput->get_xpath_element(cur,"model/StubThermal");
  // for now, just say you've done it... 
  LOG(LEV_DEBUG2) << "The StubThermal Class init(cur) function has been called";;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
StubThermal* StubThermal::deepCopy(){
  StubThermal* toRet = new StubThermal();
  toRet->copy(this);
  return toRet;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void StubThermal::copy(ThermalModel* src){
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void StubThermal::print(){
    LOG(LEV_DEBUG2) << "StubThermal Model";
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void StubThermal::transportHeat(){
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
Power StubThermal::getAvailCapacity(){
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
Temp StubThermal::getPeakTemp(){
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
Temp StubThermal::getTemp(){
}


