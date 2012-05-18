// NullFacility.cpp
// Implements the NullFacility class

#include <iostream>
#include <sstream>

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
std::string NullFacility::str() { 
  std::stringstream ss;
  ss << FacilityModel::str()
     << " converts commodity '"
     << in_commod_
     << "' into commodity '"
     << out_commod_
     << "', with inventory holding " 
     << inventory_.capacity() << " materials"
     << ", and stock holding " 
     << stocks_.capacity() << " materials";
  return ss.str();
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void NullFacility::receiveMessage(msg_ptr msg) {
  // is this a message from on high? 
  if (msg->trans().supplier()==this) {
    // file the order
    ordersWaiting_.push_front(msg);
  } else {
    throw CycException("NullFacility is not the supplier of this msg.");
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
vector<rsrc_ptr> NullFacility::removeResource(Transaction order) {
  Transaction trans = order;
  if (trans.commod() != out_commod_) {
    string err_msg = "NullFacility can only send '" + out_commod_ ;
    err_msg += + "' materials.";
    throw CycException(err_msg);
  }

  MatManifest mats;
  try {
    mats = inventory_.popQty(trans.resource()->quantity());
  } catch(CycNegQtyException err) {
    LOG(LEV_ERROR, "NulFac") << "extraction of " << trans.resource()->quantity()
                   << " kg failed. Inventory is only "
                   << inventory_.quantity() << " kg.";
  }

  return ResourceBuff::toRes(mats);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void NullFacility::addResource(Transaction trans, std::vector<rsrc_ptr> manifest) {
  try {
    stocks_.pushAll(ResourceBuff::toMat(manifest));
  } catch(CycOverCapException err) {
    LOG(LEV_ERROR, "NulFac") << "addition of resources"
                   << " to stocks failed. Stocks only has"
                   << stocks_.space() << " kg of space.";
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void NullFacility::handleTick(int time) {
  makeRequests();
  makeOffers();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void NullFacility::makeRequests() {
  double cantake = capacity_;
  if (cantake > inventory_.space()) {
    cantake = inventory_.space();
  } else if (cantake <= 0) {
    return;
  }
  LOG(LEV_INFO3, "NulFac") << name() << " ID=" << ID() << " requesting "
                           << cantake << "kg of " << in_commod_ <<".";

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
  gen_rsrc_ptr res = gen_rsrc_ptr(new GenericResource(in_commod_,"kg",amt));

  Transaction trans(this, REQUEST);
  trans.setCommod(in_commod_);
  trans.setResource(res);
  return trans;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void NullFacility::makeOffers() {
  // decide how much to offer
  double offer_amt = inventory_.quantity() + stocks_.quantity();
  if (capacity_ < stocks_.quantity()) {
    offer_amt = inventory_.quantity() + capacity_;
  }
  if (offer_amt >= inventory_.capacity()) {
    offer_amt = inventory_.capacity(); 
  }

  if (offer_amt < EPS_KG) {
    return;
  }

  // create a Resource and build transaction
  gen_rsrc_ptr res = gen_rsrc_ptr(new GenericResource(out_commod_, "kg", offer_amt));
  Transaction trans(this, OFFER);
  trans.setCommod(out_commod_);
  trans.setResource(res);

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

  double tomake = capacity_;
  if (inventory_.space() < tomake) {
    tomake = inventory_.space();
  }
  if (stocks_.quantity() < tomake) {
    tomake = stocks_.quantity();
  }

  LOG(LEV_DEBUG1, "NulFac") << "Transferring " << tomake << " kg of material from "
                  << "stocks to inventory.";
  try {
    inventory_.pushAll(stocks_.popQty(tomake));
  } catch(CycNegQtyException err) {
    LOG(LEV_ERROR, "NulFac") << "extraction of " << tomake
                   << " kg from stocks failed. Stocks is only "
                   << stocks_.quantity() << " kg.";
  } catch(CycOverCapException err) {
    LOG(LEV_ERROR, "NulFac") << "addition of " << tomake
                   << " kg from stocks to inventory failed. Inventory only has"
                   << inventory_.space() << " kg of space.";
  }

  // check what orders are waiting, 
  while(!ordersWaiting_.empty()) {
    msg_ptr order = ordersWaiting_.front();
    order.approveTransfer();
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

