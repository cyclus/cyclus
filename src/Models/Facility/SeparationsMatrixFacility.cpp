// SeparationsMatrixFacility.cpp
// Implements the SeparationsMatrixFacility class
#include <iostream>
#include <deque>

#include "SeparationsMatrixFacility.h"

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
void SeparationsMatrixFacility::init(xmlNodePtr cur)
{
	FacilityModel::init(cur);

	in_commod = out_commod = NULL; 

	/// move XML pointer to current model
	cur = XMLinput->get_xpath_element(cur,"model/SeparationsMatrixFacility");
	/// initialize any SeparationsMatrixFacility-specific datamembers here

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

  // get Stream
  string streamStr = XMLinput->get_xpath_content(cur,"Stream");

  inventory = deque<Material*>();
  stocks = deque<Material*>();
  ordersWaiting = deque<Message*>();
  ordersExecuting = ProcessLine();

  outstMF = 0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void SeparationsMatrixFacility::copy(SeparationsMatrixFacility* src)
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

// The Module below is added to match update from r240 to repository
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void SeparationsMatrixFacility::copyFreshModel(Model* src)
{
  copy((SeparationsMatrixFacility*)(src));
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void SeparationsMatrixFacility::print() 
{ 
	FacilityModel::print();
  cout << "converts commodity {"
      << in_commod->getName()
      << "} into commodity {"
      << out_commod->getName()
      << "}, and has an inventory that holds " 
      << inventory_size << " materials"
      << endl;
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SeparationsMatrixFacility::receiveMessage(Message* msg) 
{
  // is this a message from on high? 
  if(msg->getSupplierID()==this->getSN()){
    // file the order
    ordersWaiting.push_front(msg);
  }
  else {
    throw GenException("SeparationsMatrixFacility is not the supplier of this msg.");
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SeparationsMatrixFacility::sendMaterial(Message* msg, const Communicator* requester)
{
  Transaction trans = msg->getTrans();
  // it should be of out_commod Commodity type
  if(trans.commod != out_commod){
    throw GenException("SeparationsMatrixFacility can only send out_commod materials.");
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
    cout<<"SeparationsMatrixFacility "<< ID
      <<"  is sending a mat with mass: "<< newMat->getTotMass()<< endl;
  }    

	cout << "Material Before Sending to Sink" << endl;
	cout << ((FacilityModel*)(LI->getFacilityByID(trans.requesterID))) << endl;

  FacilityModel::sendMaterial(msg, toSend);

	cout << "Material After Sending to Sink" << endl;

}
    
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SeparationsMatrixFacility::receiveMaterial(Transaction trans, vector<Material*> manifest)
{

	cout << "Entered the receiveMaterial file " << endl;

  // grab each material object off of the manifest
  // and move it into the stocks.
  for (vector<Material*>::iterator thisMat=manifest.begin();
       thisMat != manifest.end();
       thisMat++)
  {
    cout<<"SeparationsFacility " << ID << " is receiving material with mass "
        << (*thisMat)->getTotMass() << endl;
    stocks.push_back(*thisMat);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SeparationsMatrixFacility::handleTick(int time)
{
  // PROCESS ORDERS EXECUTING
  separate(); // not yet fully implemented in Separations Facility

  // MAKE REQUESTS
  makeRequests();

  // MAKE OFFERS
  makeOffers();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SeparationsMatrixFacility::handleTock(int time)
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
    sendMaterial(order, ((Communicator*)LI->getFacilityByID(order->getRequesterID())));
    ordersWaiting.pop_front();
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
Mass SeparationsMatrixFacility::checkInventory(){
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
Mass SeparationsMatrixFacility::checkStocks(){
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
void SeparationsMatrixFacility::makeRequests(){
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
void SeparationsMatrixFacility::makeOffers()
{
  // decide how much to offer
  Mass offer_amt;
  Mass spotCapacity = capacity - outstMF;

  // and offer no more than the spotCapacity allows you to produce
    offer_amt = spotCapacity; 

  // there is no minimum amount a separations facility may send
  double min_amt = 0;

  // this will be an offer for free stuff
  // until cyclus has a working notion of default pricing for separated material
  double commod_price = 0;
  
  // decide what market to offer to
  Communicator* recipient = (Communicator*)(out_commod->getMarket());

  // create a message to go up to the market with these parameters
  Message* msg = new Message(up, out_commod, offer_amt, min_amt, commod_price, 
      this, recipient);

  // send it
  sendMessage(msg);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SeparationsMatrixFacility::separate()
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
