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

  // get inventory_size
  inventory_size = atof(XMLinput->get_xpath_content(cur,"inventorysize"));

  // get commodity price 
  commod_price = atof(XMLinput->get_xpath_content(cur,"commodprice"));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void SourceFacility::copy(SourceFacility* src)
{
  FacilityModel::copy(src);

  out_commod = src->out_commod;
  recipe = src->recipe;
  capacity = src->capacity;
  inventory_size = src->inventory_size;
  commod_price = src->commod_price;
  
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void SourceFacility::print() 
{ 
  FacilityModel::print();

  cout << "supplies commodity {"
      << out_commod->getName() << "} with recipe '" 
      << recipe->getName() << "' at a capacity of "
      << capacity << " " << recipe->getUnits() << " per time step."
      << " It has a max inventory of " << inventory_size << " " 
      << recipe->getUnits() <<  "." << endl;
  
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void SourceFacility::receiveMessage(Message* msg){
  // decide what the transaction is asking
  // produce the right material and send it
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void SourceFacility::handleTick(int time){
  // make offers
  // decide how much to offer
  Mass offer_amt;
  Mass inv = this->checkInventory();
  Mass possInv = inv+capacity;

  if (possInv < inventory_size){
    offer_amt = possInv;
  }
  else {
    offer_amt = inventory_size; 
  }

  // there is no minimum amount a source facility may send
  double min_amt = 0;

  // decide what market to offer to
  Communicator* recipient = (Communicator*)(out_commod->getMarket());

  // create a message to go up to the market with these parameters
  Message* msg = new Message(up, out_commod, offer_amt, min_amt, commod_price, 
      this, recipient);

  // send it
  sendMessage(msg);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void SourceFacility::handleTock(int time){
  // receive instructions from the market
  // process material
  // send material
  // make a record of all of it
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
Mass SourceFacility::checkInventory(){
	Mass total = 0;

	// Iterate through the inventory and sum the amount of whatever
  // material unit is in each object.

	deque<Material*>::iterator iter;

	for (iter = inventory.begin(); iter != inventory.end(); iter ++)
		total += (*iter)->getTotalMass();

	return total;
}
