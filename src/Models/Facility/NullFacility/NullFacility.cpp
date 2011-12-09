// NullFacility.cpp
// Implements the NullFacility class
#include "NullFacility.h"

#include <iostream>

#include "Logger.h"
#include "Logician.h"
#include "MarketModel.h"
#include "CycException.h"
#include "InputXML.h"

/*
 * TICK
 * send a request for your capacity minus your stocks.
 * offer stocks + capacity
 *
 * TOCK
 * process as much in stocks as your capacity will allow.
 * send appropriate materials to fill ordersWaiting.
 *
 * RECIEVE MATERIAL
 * put it in stocks
 *
 * SEND MATERIAL
 * pull it from inventory, fill the transaction
 */

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void NullFacility::init(xmlNodePtr cur) { 
  FacilityModel::init(cur);
  
  // move XML pointer to current model
  cur = XMLinput->get_xpath_element(cur,"model/NullFacility");

  // all facilities require commodities - possibly many
  in_commod_ = XMLinput->get_xpath_content(cur,"incommodity");
  out_commod_ = XMLinput->get_xpath_content(cur,"outcommodity");

  inventory_size_ = strtod(XMLinput->get_xpath_content(cur,"inventorysize"), NULL);
  capacity_ = strtod(XMLinput->get_xpath_content(cur,"capacity"), NULL);

  inventory_ = deque<Material*>();
  stocks_ = deque<Material*>();
  ordersWaiting_ = deque<Message*>();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void NullFacility::copy(NullFacility* src)
{

  FacilityModel::copy(src);

  in_commod_ = src->in_commod_;
  out_commod_ = src->out_commod_;
  inventory_size_ = src->inventory_size_;
  capacity_ = src->capacity_;

  inventory_ = deque<Material*>();
  stocks_ = deque<Material*>();
  ordersWaiting_ = deque<Message*>();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void NullFacility::copyFreshModel(Model* src)
{
  copy(dynamic_cast<NullFacility*>(src));
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void NullFacility::print() 
{ 
  FacilityModel::print(); 
  LOG(LEV_DEBUG2) << "    converts commodity {"
      << in_commod_
      << "} into commodity {"
      << out_commod_
      << "}, and has an inventory that holds " 
      << inventory_size_ << " materials";
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void NullFacility::receiveMessage(Message* msg)
{
  // is this a message from on high? 
  if(msg->getSupplier()==this){
    // file the order
    ordersWaiting_.push_front(msg);
  }
  else {
    throw CycException("NullFacility is not the supplier of this msg.");
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void NullFacility::sendMaterial(Message* order, const Communicator* requester)
{
  Transaction trans = order->getTrans();
  // it should be of out_commod_ commodity type
  if(trans.commod != out_commod_){
    std::string err_msg = "NullFacility can only send '" + out_commod_ ;
    err_msg += + "' materials.";
    throw CycException(err_msg);
  }

  Mass newAmt = 0;

  // pull materials off of the inventory stack until you get the trans amount

  // start with an empty manifest
  vector<Material*> toSend;

  while(trans.amount > newAmt && !inventory_.empty() ){
    Material* m = inventory_.front();

    // start with an empty material
    Material* newMat = new Material(CompMap(), 
                                  m->getUnits(),
                                  m->name(), 
                                  0, 
                                  ATOMBASED,
                                  false);

    // if the inventory obj isn't larger than the remaining need, send it as is.
    if(m->getTotMass() <= (trans.amount - newAmt)){
      newAmt += m->getTotMass();
      newMat->absorb(m);
      inventory_.pop_front();
    }
    else{ 
      // if the inventory obj is larger than the remaining need, split it.
      Material* toAbsorb = m->extractMass(trans.amount - newAmt);
      newAmt += toAbsorb->getTotMass();
      newMat->absorb(toAbsorb);
    }

    toSend.push_back(newMat);
    LOG(LEV_DEBUG2) <<"NullFacility "<< ID()
      <<"  is sending a mat with mass: "<< newMat->getTotMass();
  }    
  FacilityModel::sendMaterial( order, toSend );
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void NullFacility::receiveMaterial(Transaction trans, vector<Material*> manifest)
{
  // grab each material object off of the manifest
  // and move it into the stocks.
  for (vector<Material*>::iterator thisMat=manifest.begin();
       thisMat != manifest.end();
       thisMat++)
  {
    LOG(LEV_DEBUG2) <<"NullFacility " << ID() << " is receiving material with mass "
        << (*thisMat)->getTotMass();
    stocks_.push_back(*thisMat);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void NullFacility::handleTick(int time)
{
  // MAKE A REQUEST
  // The null facility should ask for as much stuff as it can reasonably receive.
  Mass requestAmt;
  // And it can accept amounts no matter how small
  Mass minAmt = 0;
  // check how full its inventory is
  Mass inv = this->checkInventory();
  // and how much is already in its stocks
  Mass sto = this->checkStocks(); 
  // subtract inv and sto from inventory max size to get total empty space
  Mass space = inventory_size_ - inv - sto;
  // this will be a request for free stuff
  double commod_price = 0;

  if (space == 0){
    // don't request anything
  }
  else if (space < capacity_){
    MarketModel* market = MarketModel::marketForCommod(in_commod_);
    Communicator* recipient = dynamic_cast<Communicator*>(market);
    // if empty space is less than monthly acceptance capacity
    requestAmt = space;

    // build the transaction and message
    Transaction trans;
    trans.commod = in_commod_;
    trans.min = minAmt;
    trans.price = commod_price;
    trans.amount = -requestAmt; // requests have a negative amount

    Message* request = new Message(this, recipient, trans); 
    request->setNextDest(getFacInst());
    request->sendOn();
  }
  // otherwise, the upper bound is the monthly acceptance capacity 
  // minus the amount in stocks.
  else if (space >= capacity_){
    MarketModel* market = MarketModel::marketForCommod(in_commod_);
    Communicator* recipient = dynamic_cast<Communicator*>(market);
    // if empty space is more than monthly acceptance capacity
    requestAmt = capacity_ - sto;

    // build the transaction and message
    Transaction trans;
    trans.commod = in_commod_;
    trans.min = minAmt;
    trans.price = commod_price;
    trans.amount = -requestAmt; // requests have a negative amount

    Message* request = new Message(this, recipient, trans); 
    request->setNextDest(getFacInst());
    request->sendOn();
  }
  
  // MAKE OFFERS
  // decide how much to offer
  Mass offer_amt;
  Mass possInv = inv + capacity_;

  if (possInv < inventory_size_){
    offer_amt = possInv;
  }
  else {
    offer_amt = inventory_size_; 
  }

  // there is no minimum amount a null facility may send
  double min_amt = 0;

  // decide what market to offer to
  MarketModel* market = MarketModel::marketForCommod(out_commod_);
  Communicator* recipient = dynamic_cast<Communicator*>(market);

  // build the transaction and message
  Transaction trans;
  trans.commod = out_commod_;
  trans.min = min_amt;
  trans.price = commod_price;
  trans.amount = offer_amt; // requests have a negative amount

  Message* msg = new Message(this, recipient, trans); 
  msg->setNextDest(getFacInst());
  msg->sendOn();
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void NullFacility::handleTock(int time)
{
  // at rate allowed by capacity, convert material in Stocks to out_commod_ type
  // move converted material into Inventory

  Mass complete = 0;

  while(capacity_ > complete && !stocks_.empty() ){
    Material* m = stocks_.front();

    // start with an empty material
    Material* newMat = new Material(CompMap(), 
                                  m->getUnits(),
                                  m->name(), 
                                  0, 
                                  ATOMBASED, 
                                  false);

    // if the stocks obj isn't larger than the remaining need, send it as is.
    if(m->getTotMass() <= (capacity_ - complete)){
      complete += m->getTotMass();
      newMat->absorb(m);
      stocks_.pop_front();
    }
    else{ 
      // if the stocks obj is larger than the remaining need, split it.
      Material* toAbsorb = m->extractMass(capacity_ - complete);
      complete += toAbsorb->getTotMass();
      newMat->absorb(toAbsorb);
    }

    inventory_.push_back(newMat);
  }    

  // check what orders are waiting, 
  while(!ordersWaiting_.empty()){
    Message* order = ordersWaiting_.front();
    sendMaterial(order, dynamic_cast<Communicator*>(order->getRequester()));
    ordersWaiting_.pop_front();
  }
  
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
Mass NullFacility::checkInventory(){
  Mass total = 0;

  // Iterate through the inventory and sum the amount of whatever
  // material unit is in each object.

  for (deque<Material*>::iterator iter = inventory_.begin(); 
       iter != inventory_.end(); 
       iter ++){
    total += (*iter)->getTotMass();
  }

  return total;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
Mass NullFacility::checkStocks(){
  Mass total = 0;

  // Iterate through the stocks and sum the amount of whatever
  // material unit is in_ each object.


  for (deque<Material*>::iterator iter = stocks_.begin(); 
       iter != stocks_.end(); 
       iter ++){
    total += (*iter)->getTotMass();
  }

  return total;
}

/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

extern "C" Model* construct() {
    return new NullFacility();
}

extern "C" void destruct(Model* p) {
    delete p;
}

/* ------------------- */ 

