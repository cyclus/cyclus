// NullFacility.cpp
// Implements the NullFacility class

#include <iostream>

#include "NullFacility.h"

#include "GenericResource.h"
#include "Logger.h"
#include "MarketModel.h"
#include "CycException.h"
#include "InputXML.h"

using namespace std;

/**
  TICK
  send a request for your capacity minus your stocks.
  offer stocks + capacity
 
  TOCK
  process as much in stocks as your capacity will allow.
  send appropriate materials to fill ordersWaiting.
 
  RECIEVE MATERIAL
  put it in stocks
 
  SEND MATERIAL
  pull it from inventory, fill the transaction
 */

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void NullFacility::init(xmlNodePtr cur) { 
  FacilityModel::init(cur);
  
  // move XML pointer to current model
  cur = XMLinput->get_xpath_element(cur,"model/NullFacility");

  // all facilities require commodities - possibly many
  in_commod_ = XMLinput->get_xpath_content(cur,"incommodity");
  out_commod_ = XMLinput->get_xpath_content(cur,"outcommodity");

  inventory_.setCapacity(strtod(XMLinput->get_xpath_content(cur,"inventorysize"), NULL));
  stocks_.setCapacity(strtod(XMLinput->get_xpath_content(cur,"inventorysize"), NULL));
  capacity_ = strtod(XMLinput->get_xpath_content(cur,"capacity"), NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void NullFacility::copy(NullFacility* src) {

  FacilityModel::copy(src);

  in_commod_ = src->in_commod_;
  out_commod_ = src->out_commod_;
  inventory_.setCapacity(src->inventory_.capacity());
  stocks_.setCapacity(src->stocks_.capacity());
  capacity_ = src->capacity_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void NullFacility::copyFreshModel(Model* src) {
  copy(dynamic_cast<NullFacility*>(src));
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void NullFacility::print() { 
  FacilityModel::print(); 
  LOG(LEV_DEBUG2, "NullFac") << "    converts commodity {"
      << in_commod_
      << "} into commodity {"
      << out_commod_
      << "}, and has an inventory that holds " 
      << inventory_.capacity() << " materials"
      << ", and has a stock that holds " 
      << stocks_.capacity() << " materials";
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void NullFacility::receiveMessage(msg_ptr msg) {
  // is this a message from on high? 
  if (msg->supplier()==this) {
    // file the order
    ordersWaiting_.push_front(msg);
  } else {
    throw CycException("NullFacility is not the supplier of this msg.");
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
vector<rsrc_ptr> NullFacility::removeResource(msg_ptr order) {
  Transaction trans = order->trans();
  // it should be of out_commod_ commodity type
  if (trans.commod != out_commod_) {
    string err_msg = "NullFacility can only send '" + out_commod_ ;
    err_msg += + "' materials.";
    throw CycException(err_msg);
  }

  MatManifest mats;
  try {
    mats = inventory_.popQty(trans.resource->quantity());
  } catch(CycNegQtyException err) {
    throw CycException("what is going on????????????");
  }

  return MatStore::toRes(inventory_.popQty(trans.resource->quantity()));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void NullFacility::addResource(msg_ptr msg, vector<rsrc_ptr> manifest) {
  stocks_.pushAll(MatStore::toMat(manifest));
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
  double shift = inventory_.space();
  if (shift > capacity_) {
    shift = capacity_;
  }

  double cantake = stocks_.space() + shift;

  if (cantake <= 0) {
    return;
    // don't request anything
  }

  // build the transaction and message
  MarketModel* market = MarketModel::marketForCommod(in_commod_);
  Communicator* recipient = dynamic_cast<Communicator*>(market);
  Transaction trans = buildRequestTrans(cantake);
  msg_ptr request(new Message(this, recipient, trans)); 
  request->setNextDest(facInst());
  request->sendOn();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
Transaction NullFacility::buildRequestTrans(double amt) {
  Transaction trans;
  gen_rsrc_ptr res = gen_rsrc_ptr(new GenericResource(in_commod_,"kg",amt));
  trans.commod = in_commod_;
  trans.minfrac = 0;
  trans.is_offer = false;
  trans.price = 0;
  trans.resource = res;
  return trans;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void NullFacility::makeOffers() {
  // decide how much to offer
  double offer_amt, inv;
  inv = inventory_.quantity();
  double possInv = inv + capacity_;

  if (possInv < inventory_.capacity()) {
    offer_amt = possInv;
  } else {
    offer_amt = inventory_.capacity(); 
  }

  // there is no minimum amount a null facility may send
  double min_amt = 0;
  // and it's free
  double commod_price = 0;

  // create a Resource and build transaction
  gen_rsrc_ptr res = gen_rsrc_ptr(new GenericResource(out_commod_, "kg", offer_amt));
  Transaction trans;
  trans.commod = out_commod_;
  trans.minfrac = min_amt/offer_amt;
  trans.is_offer = true;
  trans.price = commod_price;
  trans.resource = res;

  // build and send the message
  MarketModel* market = MarketModel::marketForCommod(out_commod_);
  Communicator* recipient = dynamic_cast<Communicator*>(market);
  msg_ptr msg(new Message(this, recipient, trans)); 
  msg->setNextDest(facInst());
  msg->sendOn();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void NullFacility::handleTock(int time) {
  // at rate allowed by capacity, convert material in Stocks to out_commod_ type
  // move converted material into Inventory

  double complete = 0;

  double tomake = capacity_;
  if (inventory_.space() < tomake) {
    tomake = inventory_.space();
  }
  if (stocks_.quantity() < tomake) {
    tomake = stocks_.quantity();
  }

  inventory_.pushAll(stocks_.popQty(tomake));

  // check what orders are waiting, 
  while(!ordersWaiting_.empty()) {
    msg_ptr order = ordersWaiting_.front();
    order->approveTransfer();
    ordersWaiting_.pop_front();
  }
}

/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

extern "C" Model* constructNullFacility() {
    return new NullFacility();
}

/* ------------------- */ 

