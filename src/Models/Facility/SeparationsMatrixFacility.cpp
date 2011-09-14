// SeparationsMatrixFacility.cpp
// Implements the SeparationsMatrixFacility class
#include <iostream>
#include <deque>
#include <string.h>
#include <vector>

#include "SeparationsMatrixFacility.h"

#include "Timer.h"
#include "Logician.h"
#include "GenException.h"
#include "InputXML.h"

/*
 * TICK
 * Make offers of separated material based on availabe inventory.
 * If there are ordersWaiting, prepare and send an appropriate 
 * request for spent fuel material.
 * Check stocks to determine if there is capacity to produce any extra material
 * next month. If so, process as much raw (spent fuel) stock material as
 * capacity will allow.
 *
 * TOCK
 * Send appropriate separated material from inventory to fill ordersWaiting.
 *
 * RECIEVE MATERIAL
 * Put incoming spent nuclear fuel (SNF) material into stocks
 *
 * SEND MATERIAL
 * Pull separated material from inventory based on Requests
 * Decrement ordersWaiting
 */

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void SeparationsMatrixFacility::init(xmlNodePtr cur)
{
  FacilityModel::init(cur);
 
  /// move XML pointer to current model
  cur = XMLinput->get_xpath_element(cur,"model/SeparationsMatrixFacility");
  /// initialize any SeparationsMatrixFacility-specific datamembers here

  // all facilities require commodities - possibly many
  string commod_name;
  Commodity* new_commod;
  new_commod = NULL;

  // Hack Force!!
  // Forcing Separations Matrix to know ahead of time the number of streams to be processed.
  int nodeForce = 1;

  // get incommodities
  xmlNodeSetPtr nodes = XMLinput->get_xpath_elements(cur,"incommodity");
 
  for (int i=0;i<nodes->nodeNr;i++)
  {
    xmlNodePtr commod = nodes->nodeTab[i];

    commod_name = XMLinput->get_xpath_content(cur,"incommodity");
    new_commod = LI->getCommodity(commod_name);
    if (NULL == new_commod)
      throw GenException("Input commodity '" + commod_name 
                         + "' does not exist for facility '" + getName() 
                         + "'.");
   in_commod.push_back(new_commod);
  }

  // get inventory size
  inventory_size = atof(XMLinput->get_xpath_content(cur,"inventorysize"));

  // get capacity
  capacity = atof(XMLinput->get_xpath_content(cur,"capacity"));

  // get Stream
  nodes = XMLinput->get_xpath_elements(cur,"Stream");
  // See nodeForce Hack Above

  for (int i=0;i<nodeForce;i++)
  {

    xmlNodePtr stream = nodes->nodeTab[i];

    string stream_commod = XMLinput->get_xpath_content(stream,"outcommodity");
    new_commod = NULL;
    new_commod = LI->getCommodity(stream_commod);
    if (NULL == new_commod)
      throw GenException("Output commodity '" + commod_name
                         + "' does not exist for facility '" + getName()
                         + "'.");
    out_commod.push_back(new_commod);

    int stream_Z = atoi(XMLinput->get_xpath_content(stream,"Z"));
    double stream_eff = atof(XMLinput->get_xpath_content(stream,"eff"));
    stream_set.insert(make_pair(new_commod,
                                make_pair(stream_Z, stream_eff)));
    cout << "Name = " << stream_commod << endl;
    cout << "Z = " << stream_Z << endl;
    cout << "Eff = " << stream_eff << endl;
  };

  inventory = deque<pair<Commodity*,Material*> >();
  stocks = deque<pair<Commodity*,Material*> >();
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

  inventory = deque<InSep>();
  stocks = deque<OutSep>();
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
  cout << "converts commodities {" << endl;
 
  for(vector<Commodity*>::const_iterator iter = in_commod.begin(); 
       iter != in_commod.end(); 
       iter ++){
    cout << (*iter)->getName()<< endl;
  };

  cout << "} into commodities {" << endl;

  for (vector<Commodity*>::iterator iter = out_commod.begin(); 
       iter != out_commod.end(); 
       iter ++){
    cout << (*iter)->getName()<< endl;
  }; 
  cout << "}, and has an inventory that holds " 
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
  
  Mass newAmt = 0;

  // pull materials off of the inventory stack until you get the trans amount

  // start with an empty manifest
  vector<Material*> toSend;

  while(trans.amount > newAmt && !inventory.empty() ){
    for (deque<InSep>::iterator iter = inventory.begin(); 
        iter != inventory.end(); 
        iter ++){
    Material* m = inventory.front().second;

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

  } // <- for loop {

  FacilityModel::sendMaterial(msg, toSend);

  //	cout << "Material After Sending to Sink" << endl;

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
    stocks.push_back(make_pair(trans.commod, *thisMat));
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

  
  // Handled more by separate function in handleTick than here."

  // Mass complete as in completely full inventory stored by SeparationsMatrix
  // Facility
  Mass complete = 0;

  while(capacity > complete && !stocks.empty() ){
    for (deque<OutSep>::iterator iter = stocks.begin(); 
        iter != stocks.end(); 
        iter ++){
    Material* m = stocks.front().second;

    // start with an empty material
    Material* newMat = new Material(CompMap(), 
                                  m->getUnits(),
                                  m->getName(), 
                                  0, atomBased);
    //  }
    // }
  
    
   
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
    // stocks.push_back(make_pair(trans.commod, *thisMat));
    // inventory.push_back(make_pair(commod, newMat);
  }    

  } // <- for the for loop end
  

  // fill the orders that are waiting, 
  while(!ordersWaiting.empty()){
    Message* order = ordersWaiting.front();
    sendMaterial(order, ((Communicator*)LI->getModelByID(order->getRequesterID(), FACILITY)));
    ordersWaiting.pop_front();
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
Mass SeparationsMatrixFacility::checkInventory(){
  Mass total = 0;

  // Iterate through the inventory and sum the amount of whatever
  // material unit is in each object.

  for (deque<InSep>::iterator iter = inventory.begin(); 
       iter != inventory.end(); 
       iter ++){
    total += (*iter).second->getTotMass();
  }

  return total;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
Mass SeparationsMatrixFacility::checkStocks(){
  Mass total = 0;

  // Iterate through the stocks and sum the amount of whatever
  // material unit is in each object.


  for (deque<OutSep>::iterator iter = stocks.begin(); 
       iter != stocks.end(); 
       iter ++){
    total += (*iter).second->getTotMass();
  }

  return total;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void SeparationsMatrixFacility::makeRequests(){

  for (vector<Commodity*>::iterator iter = in_commod.begin(); 
       iter != in_commod.end(); 
       iter ++){
    // The separations facility should ask for at least as much SNF as it is 
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
      int total = checkStocks();
      Communicator* recipient = (Communicator*)((*iter)->getMarket());
      // if empty space is less than monthly acceptance capacity
      requestAmt = space;
      // recall that requests have a negative amount
      Message* request = new Message(up, (*iter), -requestAmt, minAmt, 
                                       commod_price, this, recipient);
        // pass the message up to the inst
        (request->getInst())->receiveMessage(request);
    }
    // otherwise, the upper bound is the monthly acceptance capacity 
    // minus the amount in stocks.
    else if (space >= capacity){
      Communicator* recipient = (Communicator*)((*iter)->getMarket());
      // if empty space is more than monthly acceptance capacity
      requestAmt = capacity - sto;
      // recall that requests have a negative amount
      Message* request = new Message(up, (*iter), -requestAmt, minAmt, commod_price,
                                     this, recipient); 
      // pass the message up to the inst
      (request->getInst())->receiveMessage(request);
    }

  } // <- for loop
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SeparationsMatrixFacility::makeOffers()
{
  for (vector<Commodity*>::iterator iter = out_commod.begin(); 
       iter != out_commod.end(); 
       iter ++){
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
    Communicator* recipient = (Communicator*)((*iter)->getMarket());

    // create a message to go up to the market with these parameters
    Message* msg = new Message(up, (*iter), offer_amt, min_amt, commod_price, 
        this, recipient);

    // send it
    sendMessage(msg);
  }

}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SeparationsMatrixFacility::separate()
{
  // Get iterators that define the boundaries of the ordersExecuting that are 
  // currently ready.~
  int time = TI->getTime();

  pair<ProcessLine::iterator, ProcessLine::iterator> iters;

  iters = ordersExecuting.equal_range(time);

  ProcessLine::iterator curr, omega;
  curr = iters.first;
  omega = iters.second;

  // Create and send Materials corresponding to each order that's ready to go.
  while (curr != omega) 
  {
    // Get the info we need to make the separated Material.
    Message* mess = (curr->second).first;
    Material* mat = (curr->second).second;

    // Find out what we're trying to make.
    map<Iso, Atoms> compToMake = mess->getComp();

    ordersExecuting.erase(time);
  }
 
  /*
   * The section below is currently under development.  Its purpose is to do the
   * actual separations of the isotopes based on the string information.
   */

        
   // This loop will cycle through each element and then it will find out if 
   // there is anything in the stream that needs to be added to the material 
   //for separation.
   /*
   for(stream_set.second.first::iterator iter = stream_set.begin(); 
       !iter = stream_set.end(); 
       iter++){            
     firstpair = inventory.pop_front();
     Commodity* firstcommodity = firstpair.first();
     Material* firstmaterial = firstpair.second();
     // Multiply Amount of Element by Separation Efficieny and then add
     // it to the stock of material for that Element
     stocks.second((*stream_set).first) = 
     firstmaterial((*stream_set).first)*((*stream_set).second).second ++;
           
   }
   */       
}
