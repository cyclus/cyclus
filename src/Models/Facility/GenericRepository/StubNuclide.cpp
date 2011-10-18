// StubNuclide.cpp
#include <iostream>
#include <fstream>
#include <vector>
#include <time.h>

#include "GenException.h"
#include "InputXML.h"
#include "StubNuclide.h"

using namespace std;
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void StubNuclide::init(xmlNodePtr cur){
  NuclideModel::init(cur);
  // move the xml pointer to the current model
  cur = XMLinput->get_xpath_element(cur,"model/StubNuclide");
  // for now, just say you've done it... 
  cout << "The StubNuclide Class init(cur) function has been called"<< endl;;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void StubNuclide::copy(NuclideModel* src){
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void StubNuclide::print(){
    cout << "StubNuclide Model" << endl;;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void StubNuclide::absorb(Material* matToAdd)
{
  // Get the given StubNuclide's contaminant material.
  // add the material to it with the material absorb function.
  // each nuclide model should override this function
  cout << "StubNuclide is absorbing material: " << endl;
  matToAdd->print();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void StubNuclide::extract(Material* matToRem)
{
  // Get the given StubNuclide's contaminant material.
  // add the material to it with the material extract function.
  // each nuclide model should override this function
  cout << "StubNuclide" << "is extracting material: " << endl;
  matToRem->print() ;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void StubNuclide::transportNuclides(){
}


