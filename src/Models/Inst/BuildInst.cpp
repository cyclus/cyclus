// BuildInst.cpp
// Implements the BuildInst class
#include <iostream>

#include "BuildInst.h"

#include "FacilityModel.h"
#include "Model.h"

#include "Logician.h"
#include "GenException.h"
#include "InputXML.h"


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
void BuildInst::copy(BuildInst* src)
{
  InstModel::copy(src);

  facilities = src->facilities;

}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
void BuildInst::copyFreshModel(Model* src)
{

  copy((BuildInst*)src);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
void BuildInst::print() 
{ 
  InstModel::print();

  cout << " and the following permanent facilities: " << endl;
  for (vector<Model*>::iterator fac=facilities.begin(); 
       fac != facilities.end(); 
       fac++){
    cout << "\t\t* " << ((FacilityModel*)(*fac))->getFacName()
     << " (" << (*fac)->getName() << ")" << endl;
  }
};


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
bool BuildInst::pleaseBuild(Model* fac)
{
  Model* new_facility=Model::create(fac);
  // !!! We need a way to determine the new facility's name
  // Set the facility name
  string name = ((FacilityModel*)fac)->getFacName()+" 2";
  ((FacilityModel*)new_facility)->setFacName(name);
  // Set the facility's parent institution
  ((FacilityModel*)new_facility)->setInstName(this->getName());
  // Add the facility to the parent inst's list of facilities
  this->addFacility(new_facility);

  // For now, by default, return true.
  return true;
};

