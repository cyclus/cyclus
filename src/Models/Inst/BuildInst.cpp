// BuildInst.cpp
// Implements the BuildInst class
#include <iostream>

#include "BuildInst.h"

#include "FacilityModel.h"
#include "Model.h"

#include "Logician.h"
#include "Timer.h"
#include "CycException.h"
#include "InputXML.h"


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
void BuildInst::copy(BuildInst* src)
{
  InstModel::copy(src);

  facilities_ = src->facilities_;

}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
void BuildInst::copyFreshModel(Model* src)
{

  copy(dynamic_cast<BuildInst*>(src));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
void BuildInst::print() 
{ 
  InstModel::print();

  cout << " and the following permanent facilities: " << endl;
  for (vector<Model*>::iterator fac=facilities_.begin(); 
       fac != facilities_.end(); 
       fac++){
    cout << "\t\t* " << (dynamic_cast<FacilityModel*>(*fac))->getFacName()
     << " (" << (*fac)->getName() << ")" << endl;
  }
};


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
bool BuildInst::pleaseBuild(Model* fac)
{
  Model* new_facility=Model::create(fac);
  // !!! We need a way to determine the new facility's name
  // Set the facility name
  std::string name = dynamic_cast<FacilityModel*>(fac)->getFacName() + " new_build";
  dynamic_cast<FacilityModel*>(new_facility)->setFacName(name);
  // Add the facility to the parent inst's list of facilities
  this->addChild(new_facility);

  // For now, by default, return true.
  return true;
};

