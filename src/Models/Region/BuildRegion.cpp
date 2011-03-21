// BuildRegion.cpp
// Implements the BuildRegion class
#include <iostream>

#include "BuildRegion.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void BuildRegion::build(int time, InstModel* inst)
{
  // Test at arbitrary time = 2
  if (time==2){
    FacilityModel* new_facility;
    // Build one of each allowed facility
    for(set<Model*>::iterator fac=this->allowedFacilities.begin();
	fac != this->allowedFacilities.end();
	fac++){
      // make new facility a new type of *fac!!
      new_facility = new FacilityModel;
      new_facility->copyFreshModel((FacilityModel*)(*fac));
      //inst->addFacility(new_facility);
    }
  }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void BuildRegion::handleTick(int time){
  // Override RegionModel's handleTick
  for(vector<Model*>::iterator inst=institutions.begin();
      inst != institutions.end();
      inst++){
    // Call build function
    build(time,(InstModel*)(*inst));
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
