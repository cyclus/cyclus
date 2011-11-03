// StubNuclide.cpp
#include <iostream>
#include "Logger.h"
#include <fstream>
#include <vector>
#include <time.h>

#include "CycException.h"
#include "InputXML.h"
#include "StubNuclide.h"

using namespace std;
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void StubNuclide::init(xmlNodePtr cur){
  NuclideModel::init(cur);
  // move the xml pointer to the current model
  cur = XMLinput->get_xpath_element(cur,"model/StubNuclide");
  // for now, just say you've done it... 
  LOG(LEV_DEBUG2) << "The StubNuclide Class init(cur) function has been called";;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
NuclideModel* StubNuclide::copy(NuclideModel* src){
  StubNuclide* toRet = new StubNuclide();
  return (NuclideModel*)toRet;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void StubNuclide::print(){
    LOG(LEV_DEBUG2) << "StubNuclide Model";;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void StubNuclide::absorb(Material* matToAdd)
{
  // Get the given StubNuclide's contaminant material.
  // add the material to it with the material absorb function.
  // each nuclide model should override this function
  LOG(LEV_DEBUG2) << "StubNuclide is absorbing material: ";
  matToAdd->print();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void StubNuclide::extract(Material* matToRem)
{
  // Get the given StubNuclide's contaminant material.
  // add the material to it with the material extract function.
  // each nuclide model should override this function
  LOG(LEV_DEBUG2) << "StubNuclide" << "is extracting material: ";
  matToRem->print() ;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void StubNuclide::transportNuclides(){
}


