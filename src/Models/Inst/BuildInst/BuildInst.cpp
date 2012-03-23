// BuildInst.cpp
// Implements the BuildInst class
#include <iostream>
#include "Logger.h"

#include "BuildInst.h"

#include "FacilityModel.h"
#include "Model.h"

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

  LOG(LEV_DEBUG2, "none!") << " and the following permanent facilities: ";
  for (vector<Model*>::iterator fac=facilities_.begin(); 
       fac != facilities_.end(); 
       fac++){
    LOG(LEV_DEBUG2, "none!") << "        * " << (dynamic_cast<FacilityModel*>(*fac))->facName()
     << " (" << (*fac)->name() << ")";
  }
};


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
bool BuildInst::pleaseBuild(Model* fac)
{
  Model* new_facility=Model::create(fac);
  // !!! We need a way to determine the new facility's name
  // Set the facility name
  string name = dynamic_cast<FacilityModel*>(fac)->facName()+" 2";
  dynamic_cast<FacilityModel*>(new_facility)->setFacName(name);
  // Set the facility's parent institution
  dynamic_cast<FacilityModel*>(new_facility)->setInstName(this->name());
  // Add the facility to the parent inst's list of facilities
  new_facility->setParent(this);

  // For now, by default, return true.
  return true;
};


/* --------------------
   output database info
 * --------------------
 */
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
std::string BuildInst::outputDir_ = "/build";


/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

extern "C" Model* constructBuildInst() {
  return new BuildInst();
}


/* ------------------- */ 

