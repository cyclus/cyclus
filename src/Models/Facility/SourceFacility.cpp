// SourceFacility.cpp
// Implements the SourceFacility class
#include <iostream>

#include "SourceFacility.h"

#include "Logician.h"
#include "GenException.h"
#include "InputXML.h"



//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
SourceFacility::SourceFacility(){
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
SourceFacility::~SourceFacility(){
  // Delete all the Material in the inventory.
  while (!inventory.empty()) {
    Material* m = inventory.front();
    inventory.pop_front();
    delete m;
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void SourceFacility::init(xmlNodePtr cur)
{
  FacilityModel::init(cur);

  out_commod = NULL;

  /// move XML pointer to current model
  cur = XMLinput->get_xpath_element(cur,"model/SourceFacility");

  /// all facilities require commodities - possibly many
  string input_token;

  input_token = XMLinput->get_xpath_content(cur,"outcommodity");
  out_commod = LI->getCommodity(input_token);
  if (NULL == out_commod)
    throw GenException("Output commodity '" + input_token 
        + "' does not exist for facility '" + getName() 
        + "'.");

  // get recipe
  input_token = XMLinput->get_xpath_content(cur,"recipe");
  recipe = LI->getRecipe(input_token);
  if (NULL == recipe)
    throw GenException("Recipe '" + input_token 
        + "' does not exist for facility '" + getName()
        + "'.");

  // get capacity
  capacity = atof(XMLinput->get_xpath_content(cur,"capacity"));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void SourceFacility::copy(SourceFacility* src)
{
  FacilityModel::copy(src);

  out_commod = src->out_commod;
  recipe = src->recipe;
  capacity = src->capacity;
  
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void SourceFacility::print() 
{ 
  FacilityModel::print();

  cout << "supplies commodity {"
      << out_commod->getName() << "} with recipe '" 
      << recipe->getName() << "' at a capacity of "
      << capacity << " " << recipe->getUnits() << " per time step."
      << endl;
  
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void SourceFacility::handleTick(int time){
  // make offers
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void SourceFacility::handleTock(int time){
  // receive instructions from the market
  // process material
  // send material
  // make a record of all of it
}

