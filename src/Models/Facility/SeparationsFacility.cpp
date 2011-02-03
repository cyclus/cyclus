// SeparationsFacility.cpp
// Implements the SeparationsFacility class
#include <iostream>
#include <deque>

#include "SeparationsFacility.h"

#include "Timer.h"
#include "Logician.h"
#include "GenException.h"
#include "InputXML.h"

/*
 * TICK
 * If there are ordersWaiting, prepare and send an appropriate 
 * request for spent fuel material.
 * If there is capacity to produce any extra material next month
 * prepare and send an appropriate offer of separating capacity.
 *
 * TOCK
 * Process as much raw (spent fuel) stock material as capacity will allow.
 * Send appropriate materials to fill ordersWaiting.
 *
 * RECIEVE MATERIAL
 * put it in stocks
 *
 * SEND MATERIAL
 * pull it from inventory
 * decrement ordersWaiting
 */

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void SeparationsFacility::init(xmlNodePtr cur)
{
	FacilityModel::init(cur);

	in_commod = out_commod = NULL; 

	/// move XML pointer to current model
	cur = XMLinput->get_xpath_element(cur,"model/SeparationsFacility");
	/// initialize any SeparationsFacility-specific datamembers here

	// all facilities require commodities - possibly many
  string commod_name;
  Commodity* new_commod;

  commod_name = XMLinput->get_xpath_content(cur,"incommodity");
  in_commod = LI->getCommodity(commod_name);
  if (NULL == in_commod)
    throw GenException("Input commodity '" + commod_name 
                       + "' does not exist for facility '" + getName() 
                       + "'.");
  
  commod_name = XMLinput->get_xpath_content(cur,"outcommodity");
  out_commod = LI->getCommodity(commod_name);
  if (NULL == out_commod)
    throw GenException("Output commodity '" + commod_name 
                       + "' does not exist for facility '" + getName() 
                       + "'.");

  // get inventory size
  inventory_size = atof(XMLinput->get_xpath_content(cur,"inventorysize"));
  // get capacity
  capacity = atof(XMLinput->get_xpath_content(cur,"capacity"));

  inventory = deque<Material*>();
  stocks = deque<Material*>();
  ordersWaiting = deque<Message*>();
  ordersExecuting = ProcessLine();

  outstMF = 0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void SeparationsFacility::copy(SeparationsFacility* src)
{
	FacilityModel::copy(src);

  in_commod = src->in_commod;
  out_commod = src->out_commod;
  inventory_size = src->inventory_size;
  capacity = src->capacity;

  inventory = deque<Material*>();
  stocks = deque<Material*>();
  ordersWaiting = deque<Message*>();
  ordersExecuting = ProcessLine();

	outstMF = 0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void SeparationsFacility::print() 
{ 
	FacilityModel::print();
  cout << "converts commodity {"
      << in_commod->getName()
      << "} into commodity {"
      << out_commod->getName()
      << "}, and has an inventory that holds " 
      << inventory_size << " materials"
      << endl;
}


/* --------------------
 * all COMMUNICATOR classes have these members
 * --------------------
 */
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/*
void SeparationsFacility::sendMessage() 
{

};
*/

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SeparationsFacility::receiveMessage(Message* msg) 
{
  // is this a message from on high? 
  if(msg->getSupplierID()==this->getSN()){
    // file the order
    ordersWaiting.push_front(msg);
  }
  else {
    throw GenException("SeparationsEnrichmentFacility is not the supplier of this msg.");
  }
}


/* --------------------
 * all FACILITYMODEL classes have these members
 * --------------------
 */

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SeparationsFacility::sendMaterial(Transaction trans, const Communicator* receiver)
{
  // it should be of out_commod Commodity type
  if(trans.commod != out_commod){
    throw GenException("SeparationsFacility can only send out_commod materials.");
  }

  Mass newAmt = 0;

  // pull materials off of the inventory stack until you get the trans amount

  // start with an empty manifest
  vector<Material*> toSend;

  while(trans.amount > newAmt && !inventory.empty() ){
    Material* m = inventory.front();

    // start with an empty material
    Material* newMat = new Material(CompMap(), 
                                  m->getUnits(),
                                  m->getName(), 
                                  0, atomBased);

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
    cout<<"SeparationsFacility "<< ID
      <<"  is sending a mat with mass: "<< newMat->getTotMass()<< endl;
  }    
  ((FacilityModel*)(LI->getFacilityByID(trans.requesterID)))->receiveMaterial(trans, toSend);
}
    
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SeparationsFacility::receiveMaterial(Transaction trans, vector<Material*> manifest)
{
  // grab each material object off of the manifest
  // and move it into the stocks.
  for (vector<Material*>::iterator thisMat=manifest.begin();
       thisMat != manifest.end();
       thisMat++)
  {
    cout<<"SeparationsEnrichmentFacility " << ID << " is receiving material with mass "
        << (*thisMat)->getTotMass() << endl;
    stocks.push_back(*thisMat);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SeparationsFacility::handleTick(int time)
{
  // PROCESS ORDERS EXECUTING
  separate(); // not yet fully implemented in Separations Facility

  // MAKE REQUESTS
  makeRequests();

  // MAKE OFFERS
  makeOffers();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SeparationsFacility::handleTock(int time)
{
  // at rate allowed by capacity, convert material in Stocks to out_commod type
  // move converted material into Inventory

  Mass complete = 0;

  while(capacity > complete && !stocks.empty() ){
    Material* m = stocks.front();

    // start with an empty material
    Material* newMat = new Material(CompMap(), 
                                  m->getUnits(),
                                  m->getName(), 
                                  0, atomBased);

    // if the stocks obj isn't larger than the remaining need, send it as is.
    if(m->getTotMass() <= (capacity - complete)){
      complete += m->getTotMass();
      newMat->absorb(m);
      stocks.pop_front();
    }
    else{ 
      // if the stocks obj is larger than the remaining need, split it.
      Material* toAbsorb = m->extractMass(capacity - complete);
      complete += toAbsorb->getTotMass();
      newMat->absorb(toAbsorb);
    }

    inventory.push_back(newMat);
  }    

  // fill the orders that are waiting, 
  while(!ordersWaiting.empty()){
    Message* order = ordersWaiting.front();
    sendMaterial(order->getTrans(), ((Communicator*)LI->getFacilityByID(order->getRequesterID())));
    ordersWaiting.pop_front();
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
Mass SeparationsFacility::checkInventory(){
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
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
Mass SeparationsFacility::checkStocks(){
  Mass total = 0;

  // Iterate through the stocks and sum the amount of whatever
  // material unit is in each object.


  for (deque<Material*>::iterator iter = stocks.begin(); 
       iter != stocks.end(); 
       iter ++){
    total += (*iter)->getTotMass();
  }

  return total;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void SeparationsFacility::makeRequests(){
  // The enrichment facility should ask for at least as much feed as it is 
  // already committed to using this month.
  Mass requestAmt;
  // In a constrained market, it happily accepts amounts no matter how small
  Mass minAmt = 0;
  // check how full its inventory is
  Mass inv = this->checkInventory();
  // and how much is already in its stocks
  Mass sto = this->checkStocks(); 
  // subtract inv and sto from inventory max size to get total empty space
  // the request cannot exceed the space available
  Mass space = inventory_size - inv - sto;

  // Currently, no pricing information included for Separations Facility
  double commod_price = 0;
  
  // spotCapacity represents unaccounted for capacity
  Mass spotCapacity = capacity - outstMF;

  if (space == 0){
    // don't request anything
  }
  else if (space < capacity){
    Communicator* recipient = (Communicator*)(in_commod->getMarket());
    // if empty space is less than monthly acceptance capacity
    requestAmt = space;
    // recall that requests have a negative amount
    Message* request = new Message(up, in_commod, -requestAmt, minAmt, 
                                     commod_price, this, recipient);
      // pass the message up to the inst
      (request->getInst())->receiveMessage(request);
  }
  // otherwise, the upper bound is the monthly acceptance capacity 
  // minus the amount in stocks.
  else if (space >= capacity){
    Communicator* recipient = (Communicator*)(in_commod->getMarket());
    // if empty space is more than monthly acceptance capacity
    requestAmt = capacity - sto;
    // recall that requests have a negative amount
    Message* request = new Message(up, in_commod, -requestAmt, minAmt, commod_price,
                                   this, recipient); 
    // pass the message up to the inst
    (request->getInst())->receiveMessage(request);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SeparationsFacility::separate()
{
	// Get iterators that define the boundaries of the ordersExecuting that are 
	// currently ready.~
	int time = TI->getTime();

  pair<ProcessLine::iterator,
		ProcessLine::iterator> iters;
	iters = ordersExecuting.equal_range(time);

	ProcessLine::iterator curr, omega;
	curr = iters.first;
	omega = iters.second;

	// Create and send Materials corresponding to each order that's ready to go.
	while (curr != omega) 
		{

		// Get the info we need to make the enriched Material.
		Message* mess = (curr->second).first;
		Material* mat = (curr->second).second;

		// Find out what we're trying to make.
		map<Iso, Atoms> compToMake = mess->getComp();

	ordersExecuting.erase(time);
		}
}
