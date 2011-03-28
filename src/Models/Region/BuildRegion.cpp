// BuildRegion.cpp
// Implements the BuildRegion class
#include <iostream>

#include "BuildRegion.h"
#include "Model.h"
#include "FacilityModel.h"
#include "Logician.h"
#include "GenException.h"
#include "InputXML.h"


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void BuildRegion::requestBuild(Model* fac, InstModel* inst)
{
  // Request a build of fac
  inst->pleaseBuild(fac);
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void BuildRegion::handleTick(int time){
  // Override RegionModel's handleTick
  for(vector<Model*>::iterator inst=institutions.begin();
      inst != institutions.end();
      inst++){
    
    // testing at arbitrary time = 2
    if (time==2){
      // Call requestBuild function
      Model* fac_to_build=LI->getFacilityByName("BackEnd");
      requestBuild(fac_to_build,(InstModel*)(*inst));
    }

    // Pass the handleTick onto each institution
    ((InstModel*)(*inst))->handleTick(time);
  }
}

void BuildRegion::handleTock(int time){
  // tell all of the institution models to handle the tick
  for(vector<Model*>::iterator inst=institutions.begin();
      inst != institutions.end();
      inst++){
    ((InstModel*)(*inst))->handleTock(time);
  }
}
