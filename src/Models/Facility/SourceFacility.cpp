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

  inventory = deque<Material*>();
  ordersWaiting = deque<Message*>();
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
  
  inventory = deque<Material*>();
  ordersWaiting = deque<Message*>();
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

  // is this a message from on high? 
  if(msg->getSupplierID()==this->getSN()){
    // file the order
    ordersWaiting.push_front(msg);
  }
  else {
    throw GenException("SourceFacility is not the supplier of this msg.");
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void SourceFacility::sendMaterial(Transaction trans, const Communicator* requester)
{
  Mass newAmt = 0;

  // pull materials off of the inventory stack until you get the trans amount

  // start with an empty manifest
  vector<Material*> toSend;

  while(trans.amount > newAmt && !inventory.empty() ){
    // start with an empty material
    Material* newMat = new Material(recipe->getComp(), 
                                  recipe->getUnits(),
                                  recipe->getName(), 
                                  0);

    Material* m = inventory.front();
    // if the inventory obj isn't larger than the remaining need, send it as is.
    if(m->getTotMass() <= (trans.amount - newAmt)){
      newAmt += m->getTotMass();
      newMat->absorb(m);
      inventory.pop_front();
    }
    else{ 
      // if the inventory obj is larger than the remaining need, split it.
      Material* toAbsorb = m->extractMass(trans.amount - newAmt);
      newAmt += toAbsorb->getTotMass();
      newMat->absorb(toAbsorb);
    }

    toSend.push_back(newMat);
    cout<<"SourceFacility "<< ID
      <<"  is sending a mat with mass: "<< newMat->getTotMass()<< endl;
  }    
  ((FacilityModel*)(LI->getFacilityByID(trans.requesterID)))->receiveMaterial(trans, toSend);
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
  // if there's room in the inventory, process material at capacity
  Mass space = inventory_size - this->checkInventory();
  if(capacity <= space){
    // add a material the size of the capacity to the inventory
    Material* newMat = new Material(recipe->getComp(), 
                                    recipe->getUnits(), 
                                    recipe->getName(),
                                    capacity);
    inventory.push_front(newMat);
  }
  else if (space < capacity && space > 0){
    // add a material that fills the inventory
    Material* newMat = new Material(recipe->getComp(), 
                                    recipe->getUnits(), 
                                    recipe->getName(),
                                    space);
    inventory.push_front(newMat);
  }
  // check what orders are waiting, 
  // pull materials off of the inventory stack until you get the trans amount
  while(!ordersWaiting.empty()){
    Message* order = ordersWaiting.front();
    sendMaterial(order->getTrans(), ((Communicator*)LI->getFacilityByID(order->getRequesterID())));
    ordersWaiting.pop_front();
  }
  // Maybe someday it will record things.
  // For now, lets just print out what we have at each timestep.
  cout << "SourceFacility " << this->getSN();
  cout << " is holding " << this->checkInventory();
  cout << " units of material at the close of month " << time; 
  cout << "." << endl;

}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
Mass SourceFacility::checkInventory(){
	Mass total = 0;

	// Iterate through the inventory and sum the amount of whatever
  // material unit is in each object.


	for (deque<Material*>::iterator iter = inventory.begin(); 
       iter != inventory.end(); 
       iter ++){
		total += (*iter)->getTotMass();
  }

	return total;
}
