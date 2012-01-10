// NullFacility.cpp
// Implements the NullFacility class
#include "NullFacility.h"

#include <iostream>

#include "GenericResource.h"
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
void NullFacility::receiveMessage(Message* msg) {
  // is this a message from on high? 
  if (msg->getSupplier()==this) {
    // file the order
    ordersWaiting_.push_front(msg);
  } else {
    throw CycException("NullFacility is not the supplier of this msg.");
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void NullFacility::sendMaterial(Message* order, const Communicator* requester) {
  Transaction trans = order->getTrans();
  // it should be of out_commod_ commodity type
  if (trans.commod != out_commod_) {
    std::string err_msg = "NullFacility can only send '" + out_commod_ ;
    err_msg += + "' materials.";
    throw CycException(err_msg);
  }

  double newAmt = 0;

  // pull materials off of the inventory stack until you get the trans amount

  // start with an empty manifest
  vector<Material*> toSend;

  while (trans.resource->getQuantity() > newAmt && !inventory_.empty() ) {
    Material* m = inventory_.front();

    // if the inventory obj isn't larger than the remaining need, send it as is.
    if (m->getQuantity() <= (trans.resource->getQuantity() - newAmt)) {
      newAmt += m->getQuantity();
      inventory_.pop_front();
    } else {
      // if the inventory obj is larger than the remaining need, split it.
      Material* leftover = m->extract(trans.resource->getQuantity() - newAmt);
      newAmt += m->getQuantity();
      inventory_.pop_front();
      inventory_.push_back(leftover);
    }

    toSend.push_back(m);
    LOG(LEV_DEBUG2) <<"NullFacility "<< ID()
      <<"  is sending a mat with mass: "<< m->getQuantity();
  }    
  FacilityModel::sendMaterial( order, toSend );
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void NullFacility::receiveMaterial(Transaction trans, vector<Material*> manifest) {
  // grab each material object off of the manifest
  // and move it into the stocks.
  for (vector<Material*>::iterator thisMat=manifest.begin();
       thisMat != manifest.end();
       thisMat++) {
    LOG(LEV_DEBUG2) <<"NullFacility " << ID() << " is receiving material with mass "
        << (*thisMat)->getQuantity();
    stocks_.push_back(*thisMat);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void NullFacility::handleTick(int time) {
  // MAKE A REQUEST
  makeRequests();
  // MAKE OFFERS 
  makeOffers();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void NullFacility::makeRequests() {
  // The null facility should ask for as much stuff as it can reasonably receive.
  double requestAmt;
  // And it can accept amounts no matter how small
  double minAmt = 0;
  // check how full its inventory is
  double inv = this->checkInventory();
  // and how much is already in its stocks
  double sto = this->checkStocks(); 
  // subtract inv and sto from inventory max size to get total empty space
  double space = inventory_size_ - inv - sto;
  // this will be a request for free stuff
  double commod_price = 0;

  if (space == 0) {
    // don't request anything
  } else if (space < capacity_) {
    MarketModel* market = MarketModel::marketForCommod(in_commod_);
    Communicator* recipient = dynamic_cast<Communicator*>(market);
    // if empty space is less than monthly acceptance capacity
    requestAmt = space;

    // create a resource
    GenericResource* request_res = new GenericResource(in_commod_,"kg",requestAmt);

    // build the transaction and message
    Transaction trans;
    trans.commod = in_commod_;
    trans.minfrac = minAmt/requestAmt;
    trans.is_offer = false;
    trans.price = commod_price;
    trans.resource = request_res;

    Message* request = new Message(this, recipient, trans); 
    request->setNextDest(getFacInst());
    request->sendOn();
  // otherwise, the upper bound is the monthly acceptance capacity 
  // minus the amount in stocks.
  } else if (space >= capacity_) {
    MarketModel* market = MarketModel::marketForCommod(in_commod_);
    Communicator* recipient = dynamic_cast<Communicator*>(market);
    // if empty space is more than monthly acceptance capacity
    requestAmt = capacity_ - sto;

    // create a resource
    GenericResource* request_res = new GenericResource(in_commod_,"kg",requestAmt);

    // build the transaction and message
    Transaction trans;
    trans.commod = in_commod_;
    trans.minfrac = minAmt/requestAmt;
    trans.is_offer = false;
    trans.price = commod_price;
    trans.resource = request_res;

    Message* request = new Message(this, recipient, trans); 
    request->setNextDest(getFacInst());
    request->sendOn();
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void NullFacility::makeOffers() {

  // decide how much to offer
  double offer_amt, inv;
  inv = this->checkInventory();
  double possInv = inv + capacity_;

  if (possInv < inventory_size_) {
    offer_amt = possInv;
  } else {
    offer_amt = inventory_size_; 
  }

  // there is no minimum amount a null facility may send
  double min_amt = 0;
  // and it's free
  double commod_price = 0;

  // decide what market to offer to
  MarketModel* market = MarketModel::marketForCommod(out_commod_);
  Communicator* recipient = dynamic_cast<Communicator*>(market);

  // create a Resource
  GenericResource* offer_res = new GenericResource(out_commod_, "kg", offer_amt);

  // build the transaction and message
  Transaction trans;
  trans.commod = out_commod_;
  trans.minfrac = min_amt/offer_amt;
  trans.is_offer = true;
  trans.price = commod_price;
  trans.resource = offer_res;

  Message* msg = new Message(this, recipient, trans); 
  msg->setNextDest(getFacInst());
  msg->sendOn();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void NullFacility::handleTock(int time) {
  // at rate allowed by capacity, convert material in Stocks to out_commod_ type
  // move converted material into Inventory

  double complete = 0;

  // while there's still capacity left and stuff in the stocks
  while(capacity_ > complete && !stocks_.empty() ) {
    Material* m = stocks_.front();

    if(m->getQuantity() <= (capacity_ - complete)){
      // if the mass of the material is less than the remaining capacity
      complete += m->getQuantity();
      stocks_.pop_front();
    } else { 
      // if the mass is too bit, split the stocks object 
      Material* leftover = m->extract(capacity_ - complete);
      complete += m->getQuantity();
      stocks_.pop_front();
      stocks_.push_back(leftover);
    }

    inventory_.push_back(m);
  }    

  // check what orders are waiting, 
  while(!ordersWaiting_.empty()) {
    Message* order = ordersWaiting_.front();
    sendMaterial(order, dynamic_cast<Communicator*>(order->getRequester()));
    ordersWaiting_.pop_front();
  }

  // call the facility model's handle tock last 
  // to check for decommissioning
  FacilityModel::handleTock(time);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
double NullFacility::checkInventory() {
  double total = 0;

  // Iterate through the inventory and sum the amount of whatever
  // material unit is in each object.

  for (deque<Material*>::iterator iter = inventory_.begin(); 
       iter != inventory_.end(); 
       iter ++){
    total += (*iter)->getQuantity();
  }

  return total;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
double NullFacility::checkStocks(){
  double total = 0;

  // Iterate through the stocks and sum the amount of whatever
  // material unit is in_ each object.


  for (deque<Material*>::iterator iter = stocks_.begin(); 
       iter != stocks_.end(); 
       iter ++){
    total += (*iter)->getQuantity();
  }

  return total;
}

/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

extern "C" Model* constructNullFacility() {
    return new NullFacility();
}

extern "C" void destructNullFacility(Model* p) {
    delete p;
}

/* ------------------- */ 

