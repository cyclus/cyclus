// BuildRegion.cpp
// Implements the BuildRegion class

#include "BuildRegion.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void BuildRegion::requestBuild(Model* fac, InstModel* inst)
{
  // Request that Institution inst build Facility fac
  inst->pleaseBuild(fac);
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void BuildRegion::handleTick(int time){
  // Override RegionModel's handleTick
  for(vector<Model*>::iterator inst=institutions.begin();
      inst != institutions.end();
      inst++){
    
    // !!! testing at arbitrary time = 2
    // !!! need to create a build_map -> look at DeployInst.
    if (time==2){
      // Call requestBuild function
      // !!! must develop a method to determine which facility to build
      Model* fac_to_build=LI->getFacilityByName("BackEnd");
      requestBuild(fac_to_build,(InstModel*)(*inst));
    }

    // Pass the handleTick onto each institution
    ((InstModel*)(*inst))->handleTick(time);
  }
}
