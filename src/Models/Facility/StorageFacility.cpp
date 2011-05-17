// StorageFacility.cpp
// Implements the StorageFacility class
#include <iostream>

#include "StorageFacility.h"

#include "Logician.h"
#include "GenException.h"
#include "InputXML.h"
#include "Timer.h"

/*
 * TICK
 * send a request for your capacity minus your stocks.
 * offer materials that have exceeded their residence times
 * offer them
 *
 * TOCK
 * send appropriate materials to fill ordersWaiting.
 *
 * RECIEVE MATERIAL
 * put it in stocks
 * add it to the deque of release times
 *
 * SEND MATERIAL
 * pull it from inventory, fill the transaction
 */


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void StorageFacility::getInitialState()
{
//     cout<<"StorageFacility " << ID << " is receiving material with mass "
//         << (*thisMat)->getTotMass() << endl;
//     stocks.push_back(*thisMat);
//     entryTimes.push_back(make_pair(TI->getTime(), *thisMat ));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void StorageFacility::init(xmlNodePtr cur)
{ 
  FacilityModel::init(cur);

  incommod =  NULL; 
  
  // move XML pointer to current model
  cur = XMLinput->get_xpath_element(cur,"model/StorageFacility");

  // all facilities require commodities - possibly many
  string commod_name;
  Commodity* new_commod;
  
  commod_name = XMLinput->get_xpath_content(cur,"incommodity");
  incommod = LI->getCommodity(commod_name);
  if (NULL == incommod)
    throw GenException("Input commodity '" + commod_name 
                       + "' does not exist for facility '" + getName() 
                       + "'.");
  
  inventory_size = atof(XMLinput->get_xpath_content(cur,"inventorysize"));
  capacity = atof(XMLinput->get_xpath_content(cur,"capacity"));
  residence_time = atof(XMLinput->get_xpath_content(cur,"residencetime"));


  inventory = deque<Material*>();
  stocks = deque<Material*>();
  ordersWaiting = deque<Message*>();

  this -> getInitialState();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void StorageFacility::copy(StorageFacility* src)
{

  FacilityModel::copy(src);

  incommod = src->incommod;
  inventory_size = src->inventory_size;
  capacity = src->capacity;
  residence_time = src->residence_time;

  inventory = deque<Material*>();
  stocks = deque<Material*>();
  ordersWaiting = deque<Message*>();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void StorageFacility::copyFreshModel(Model* src)
{
  copy((StorageFacility*)(src));
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void StorageFacility::print() 
{ 
  FacilityModel::print(); 
  cout << "stores commodity {"
      << incommod->getName()
      << "}, for a minimum time of " 
      << residence_time 
      << " months and has an inventory that holds " 
      << inventory_size << " materials."
      << endl;
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void StorageFacility::receiveMessage(Message* msg)
{
  // is this a message from on high? 
  if(msg->getSupplierID()==this->getSN()){
    // file the order
    ordersWaiting.push_front(msg);
  }
  else {
    throw GenException("StorageFacility is not the supplier of this msg.");
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void StorageFacility::sendMaterial(Message* order, const Communicator* requester)
{
  Transaction trans = order->getTrans();
  // it should be of incommod Commodity type
  if(trans.commod != incommod){
    throw GenException("StorageFacility can only send incommodity type materials.");
  }
  // pull materials off of the inventory stack until you get the trans amount
  Mass complete = 0;

  // start with an empty manifest
  vector<Material*> toSend;

  while(trans.amount > complete && !inventory.empty() ){
    Material* m = inventory.front();

    // if the inventory obj isn't larger than the remaining need, send it as is.
    if(m->getTotMass() <= (capacity - complete)){
      complete += m->getTotMass();
      toSend.push_back(m);
      cout<<"StorageFacility "<< ID
        <<"  is sending a mat with mass: "<< m->getTotMass()<< endl;
      inventory.pop_front();
    }
    else{ 
      // if the inventory obj is larger than the remaining need, split it.
      // start with an empty material
      Material* newMat = new Material(CompMap(), 
          m->getUnits(),
          m->getName(), 
          0, atomBased);
      Material* toAbsorb = m->extractMass(capacity - complete);
      complete += toAbsorb->getTotMass();
      newMat->absorb(toAbsorb);
      toSend.push_back(newMat);
      cout<<"StorageFacility "<< ID
        <<"  is sending a mat with mass: "<< newMat->getTotMass()<< endl;
    };
  };    

  FacilityModel::sendMaterial( order, toSend );
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void StorageFacility::receiveMaterial(Transaction trans, vector<Material*> manifest)
{
  // grab each material object off of the manifest
  // and move it into the stocks.
  // also record its entry time map in entryTimes deque
  for (vector<Material*>::iterator thisMat=manifest.begin();
       thisMat != manifest.end();
       thisMat++)
  {
    cout<<"StorageFacility " << ID << " is receiving material with mass "
        << (*thisMat)->getTotMass() << endl;
    stocks.push_back(*thisMat);
    entryTimes.push_back(make_pair(TI->getTime(), *thisMat ));
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void StorageFacility::handleTick(int time)
{

  // MAKE A REQUEST
  // The storage facility should ask for as much stuff as it can reasonably receive.
  Mass requestAmt;
  // And it can accept amounts no matter how small
  Mass minAmt = 0;
  // check how full its inventory is
  Mass inv = this->checkInventory();
  // and how much is already in its stocks
  Mass sto = this->checkStocks(); 
  // subtract inv and sto from inventory max size to get total empty space
  Mass space = inventory_size - inv - sto;
  // this will be a request for free stuff
  double commod_price = 0;

  if (space == 0){
    // don't request anything
  }
  else if (space < capacity){
    Communicator* recipient = (Communicator*)(incommod->getMarket());
    // if empty space is less than monthly acceptance capacity
    requestAmt = space;
    // recall that requests have a negative amount
    Message* request = new Message(up, incommod, -requestAmt, minAmt, 
                                     commod_price, this, recipient);
      // pass the message up to the inst
      (request->getInst())->receiveMessage(request);
  }
  // otherwise, the upper bound is the monthly acceptance capacity 
  // minus the amount in stocks.
  else if (space >= capacity){
    Communicator* recipient = (Communicator*)(incommod->getMarket());
    // if empty space is more than monthly acceptance capacity
    requestAmt = capacity - sto;
    // recall that requests have a negative amount
    Message* request = new Message(up, incommod, -requestAmt, minAmt, commod_price,
                                   this, recipient); 
    // pass the message up to the inst
    (request->getInst())->receiveMessage(request);
  }
  
  // MAKE OFFERS
  // anything in the inventory is old enough to leave 
  Mass offer_amt;
  Mass possInv = inv;

  // if the inventory isn't full, then offer what you have
  if (possInv < inventory_size){
    offer_amt = possInv;
  }
  else {
    offer_amt = inventory_size; 
  }

  // there is no minimum amount a storage facility may send
  double min_amt = 0;

  // decide what market to offer to
  Communicator* recipient = (Communicator*)(incommod->getMarket());

  // create a message to go up to the market with these parameters
  Message* msg = new Message(up, incommod, offer_amt, min_amt, commod_price, 
      this, recipient);

  // send it
  sendMessage(msg);
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void StorageFacility::handleTock(int time)
{
  // CHECK MATERIAL AGES
  // Some materials in the stocks have reached their minimum residence time. 
  // put them in the inventory
  bool someOld = true;
  while( someOld == true && !stocks.empty()){
    Material* oldEnough = stocks.front();
    if(TI->getTime() - entryTimes.front().first >= residence_time ){
        entryTimes.pop_front();
        // Here is is where we could add a case switch between sending
        // youngest or oldest material first
        inventory.push_back(oldEnough);
        stocks.pop_front();
      };
  };

  // check what orders are waiting, 
  while(!ordersWaiting.empty()){
    Message* order = ordersWaiting.front();
    sendMaterial(order, ((Communicator*)LI->getFacilityByID(order->getRequesterID())));
    ordersWaiting.pop_front();
  }
  
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
Mass StorageFacility::checkInventory(){
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
Mass StorageFacility::checkStocks(){
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


