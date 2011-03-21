// BuildRegion.cpp
// Implements the BuildRegion class
#include <iostream>

#include "BuildRegion.h"

// We include the logician to access the region's InstModels
#include "Logician.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void BuildRegion::build()
{
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void BuildRegion::handleTick(int time){
  // tell all of the institution models to handle the tick
  for(vector<Model*>::iterator inst=institutions.begin();
      inst != institutions.end();
      inst++){
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
