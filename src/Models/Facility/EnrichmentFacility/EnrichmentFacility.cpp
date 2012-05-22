// EnrichmentFacility.cpp
// Implements the EnrichmentFacility class
#include <iostream>
#include <sstream>
#include "Logger.h"
#include <deque>

#include "EnrichmentFacility.h"

#include "Timer.h"
#include "CycException.h"
#include "InputXML.h"
#include "MarketModel.h"
#include "GenericResource.h"
#include "CompMap.h"

using namespace std;

/**
  TICK
  If there are ordersWaiting, prepare and send an appropriate 
  request for raw material.
  If there is capacity to produce any extra material next month
  prepare and send an appropriate offer of SWUs.
 
  TOCK
  Process as much raw stock material as capacity will allow.
  Send appropriate materials to fill ordersWaiting.
 
 
  RECIEVE MATERIAL
  put it in stocks
 
  SEND MATERIAL
  pull it from inventory
  decrement ordersWaiting
 */

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void EnrichmentFacility::init(xmlNodePtr cur){ 
  FacilityModel::init(cur);
  
  // move XML pointer to current model
  cur = XMLinput->get_xpath_element(cur,"model/EnrichmentFacility");

  // all facilities require commodities - possibly many
  in_commod_ = XMLinput->get_xpath_content(cur,"incommodity");
  out_commod_ = XMLinput->get_xpath_content(cur,"outcommodity");

  // get inventory size
  inventory_size_ = strtod(XMLinput->get_xpath_content(cur,"inventorysize"), NULL);
  // get capacity_
  capacity_ = strtod(XMLinput->get_xpath_content(cur,"capacity"), NULL);
  // get default tails fraction
  default_xw_ = strtod(XMLinput->get_xpath_content(cur,"tailsassay"), NULL);

  inventory_ = deque<mat_rsrc_ptr>();
  stocks_ = deque<mat_rsrc_ptr>();
  ordersWaiting_ = deque<msg_ptr>();
  ordersExecuting_ = ProcessLine();

  outstMF_ = 0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void EnrichmentFacility::copy(EnrichmentFacility* src){

  FacilityModel::copy(src);

  in_commod_ = src->in_commod_;
  out_commod_ = src->out_commod_;
  inventory_size_ = src->inventory_size_;
  capacity_ = src->capacity_;
  default_xw_ = src->default_xw_;

  inventory_ = deque<mat_rsrc_ptr>();
  stocks_ = deque<mat_rsrc_ptr>();
  ordersWaiting_ = deque<msg_ptr>();
  ordersExecuting_ = ProcessLine();

  outstMF_ = 0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void EnrichmentFacility::copyFreshModel(Model* src){
  copy(dynamic_cast<EnrichmentFacility*>(src));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
std::string EnrichmentFacility::str() { 
  std::stringstream ss;
  ss << FacilityModel::str()
     << " converts commodity '" << in_commod_
     << "' into commodity '" << out_commod_
     << "', with inventory holding " 
     << inventory_size_ << " materials.";
  return ss.str();
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void EnrichmentFacility::receiveMessage(msg_ptr msg){
  // is this a message from on high? 
  if(msg->supplier()==this){
    // file the order
    ordersWaiting_.push_front(msg);
  }
  else {
    throw CycException("EnrichmentFacility is not the supplier of this msg.");
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
vector<rsrc_ptr> EnrichmentFacility::removeResource(msg_ptr msg) {
  Transaction trans = msg->trans();
  // it should be of out_commod_ Commodity type
  if(trans.commod != out_commod_){
    throw CycException("EnrichmentFacility can only send '" +  out_commod_ + 
                       "' materials.");
  }

  double newAmt = 0;

  // pull materials off of the inventory stack until you get the trans amount

  // start with an empty manifest
  vector<rsrc_ptr> toSend;

  while(trans.resource->quantity() > newAmt && !inventory_.empty() ) {
    mat_rsrc_ptr m = inventory_.front();

    // start with an empty material
    mat_rsrc_ptr newMat = mat_rsrc_ptr(new Material());

    // if the inventory obj isn't larger than the remaining need, send it as is.
    if(m->quantity() <= (trans.resource->quantity() - newAmt)) {
      newAmt += m->quantity();
      newMat->absorb(m);
      inventory_.pop_front();
    } else { 
      // if the inventory obj is larger than the remaining need, split it.
      mat_rsrc_ptr toAbsorb = m->extract(trans.resource->quantity() - newAmt);
      newMat->absorb(toAbsorb);
      newAmt += toAbsorb->quantity();
    }

    toSend.push_back(newMat);
    LOG(LEV_DEBUG2, "none!") <<"EnrichmentFacility "<< ID()
      <<"  is sending a mat with mass: "<< newMat->quantity();
  }    
  return toSend;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void EnrichmentFacility::addResource(msg_ptr msg, std::vector<rsrc_ptr> manifest) {
  // grab each material object off of the manifest
  // and move it into the stocks.
  for (vector<rsrc_ptr>::iterator thisMat=manifest.begin();
       thisMat != manifest.end();
       thisMat++) {
    LOG(LEV_DEBUG2, "none!") <<"EnrichmentFacility " << ID() << " is receiving material with mass "
        << (*thisMat)->quantity();
    stocks_.push_back(boost::dynamic_pointer_cast<Material>(*thisMat));
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void EnrichmentFacility::handleTick(int time){
  // PROCESS ORDERS EXECUTING
  enrich();

  // MAKE REQUESTS
  makeRequests();

  // MAKE OFFERS
  makeOffers();

}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void EnrichmentFacility::handleTock(int time) {
  // at rate allowed by capacity_, convert material in Stocks to out_commod_ type
  // move converted material into Inventory

  double complete = 0;

  while(capacity_ > complete && !stocks_.empty() ){
    mat_rsrc_ptr m = stocks_.front();

    // start with an empty material
    mat_rsrc_ptr newMat = mat_rsrc_ptr(new Material());

    // if the stocks obj isn't larger than the remaining need, send it as is.
    if(m->quantity() <= (capacity_ - complete)){
      complete += m->quantity();
      newMat->absorb(m);
      stocks_.pop_front();
    }
    else{ 
      // if the stocks obj is larger than the remaining need, split it.
      mat_rsrc_ptr toAbsorb = m->extract(capacity_ - complete);
      complete += toAbsorb->quantity();
      newMat->absorb(toAbsorb);
    }

    inventory_.push_back(newMat);
  }    

  // fill the orders that are waiting, 
  while(!ordersWaiting_.empty()){
    msg_ptr order = ordersWaiting_.front();
    order->approveTransfer();
    ordersWaiting_.pop_front();
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
double EnrichmentFacility::checkInventory(){
  double total = 0;

  // Iterate through the inventory and sum the amount of whatever
  // material unit is in each object.

  for (deque<mat_rsrc_ptr>::iterator iter = inventory_.begin(); 
       iter != inventory_.end(); 
       iter ++){
    total += (*iter)->quantity();
  }

  return total;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
double EnrichmentFacility::checkStocks(){
  double total = 0;

  // Iterate through the stocks and sum the amount of whatever
  // material unit is in each object.


  for (deque<mat_rsrc_ptr>::iterator iter = stocks_.begin(); 
       iter != stocks_.end(); 
       iter ++){
    total += (*iter)->quantity();
  }

  return total;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void EnrichmentFacility::makeRequests(){
  // The enrichment facility should ask for at least as much feed as it is 
  // already committed to using this month.
  double requestAmt;
  // In a constrained market, it happily accepts amounts no matter how small
  double minAmt = 0;
  // check how full its inventory is
  double inv = this->checkInventory();
  // and how much is already in its stocks
  double sto = this->checkStocks(); 
  // subtract inv and sto from inventory max size to get total empty space
  // the request cannot exceed the space available
  double space = inventory_size_ - inv - sto;

  // this will be a request for free stuff
  // until cyclus has a working notion of default pricing
  // It would be nice to incorporate eqn 3.18 of Cochran & Tsoulfanidis
  // "The Nuclear Fuel Cycle : Analysis and Management" 
  double commod_price = 0;
  
  // spotCapacity represents unaccounted for capacity_
  double spotCapacity = capacity_ - outstMF_;

  if (space == 0){
    // don't request anything
  }
  else {
    // if empty space is less than monthly acceptance capacity_
    if (space < capacity_){
      requestAmt = space;

    // otherwise, the upper bound is the monthly acceptance capacity_ 
    // minus the amount in stocks.
    } else if (space >= capacity_){
      requestAmt = capacity_ - sto;
    }

    MarketModel* market = MarketModel::marketForCommod(in_commod_);
    Communicator* recipient = dynamic_cast<Communicator*>(market);

    // request a generic object
    rsrc_ptr req_res = gen_rsrc_ptr(new GenericResource(in_commod_,"kg",requestAmt));

    // build the transaction and message
    Transaction trans;
    trans.commod = in_commod_;
    trans.is_offer = false;
    trans.minfrac = minAmt/requestAmt;
    trans.price = commod_price;
    trans.resource = req_res;

    msg_ptr request(new Message(this, recipient, trans)); 
    request->setNextDest(facInst());
    request->sendOn();
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void EnrichmentFacility::makeOffers() {
  // decide how much to offer
  double offer_amt;
  double spotCapacity = capacity_ - outstMF_;

  // and offer no more than the spotCapacity_ allows you to produce
  offer_amt = spotCapacity; 

  // there is no minimum amount a enrichment facility may send
  double min_amt = 0;

  // this will be an offer for free stuff
  // until cyclus has a working notion of default pricing
  // It would be nice to incorporate eqn 3.18 of Cochran & Tsoulfanidis
  // "The Nuclear Fuel Cycle : Analysis and Management" 
  double commod_price = 0;
  
  // decide what market to offer to
  MarketModel* market = MarketModel::marketForCommod(out_commod_);
  Communicator* recipient = dynamic_cast<Communicator*>(market);

  gen_rsrc_ptr offer_res = gen_rsrc_ptr(new GenericResource(out_commod_,"SWUs",offer_amt));

  // build the transaction and message
  Transaction trans;
  trans.commod = out_commod_;
  trans.is_offer = true;;
  trans.minfrac = min_amt/offer_amt;
  trans.price = commod_price;
  trans.resource = offer_res;

  msg_ptr msg(new Message(this, recipient, trans)); 
  msg->setNextDest(facInst());
  msg->sendOn();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void EnrichmentFacility::enrich() {
  // Get iterators that define the boundaries of the ordersExecuting that are 
  // currently ready.~
  int time = TI->time();

  pair<ProcessLine::iterator,
    ProcessLine::iterator> iters;
  iters = ordersExecuting_.equal_range(time);

  ProcessLine::iterator curr, omega;
  curr = iters.first;
  omega = iters.second;

  // Create and send Materials corresponding to each order that's ready to go.
  while (curr != omega) {

    // Get the info we need to make the enriched Material.
    msg_ptr mess = (curr->second).first;
    mat_rsrc_ptr mat = (curr->second).second;

    IsoVector mat_iso, vecToMake;
    mat_iso = mat->isoVector();

    // Find out what we're trying to make.
    //
    try {
      vecToMake = boost::dynamic_pointer_cast<Material>(mess->resource())->isoVector();
    } catch (exception& e) {
      string err = "The Enrichment Facility may only receive a Material-type Resource";
      throw CycException(err);
    }

    // Do the enrichment math.
    double xp = vecToMake.massFraction(922350);
    double P = vecToMake.quantity() * xp;
    double xf = mat_iso.massFraction(922350);
    double F = mat.quantity() * xf;
    double W = F - P;
    double xw = (F * xf - P * xp) / W;

    // Make the product
    double atoms235;
    double atoms238;
    double atoms19;
    double grams92;

    // in this moment, we assume that P is in tons... KDHFLAG
    grams92 = P * 1E6;
    atoms235 = grams92 * xp * AVOGADRO / 235;
    atoms238 = grams92 * (1 - xp) * AVOGADRO / 238;

    // The stoich for this one's easy:
    atoms19 = (atoms235 + atoms238) * 6;

    CompMapPtr pComp = CompMapPtr(new CompMap(ATOM));
    (*pComp)[922350] = atoms235;
    (*pComp)[922380] = atoms238;
    (*pComp)[90190] = atoms19;

    mat_rsrc_ptr theProd = mat_rsrc_ptr(new Material(pComp));
    theProd->setQuantity(mat->quantity());

    // in this moment, we assume that P is in tons... KDHFLAG
    grams92 = W * 1E6;
    atoms235 = grams92 * xw * AVOGADRO / 235;
    atoms238 = grams92 * (1 - xw) * AVOGADRO / 238;

    // The stoich for this one's easy:
    atoms19 = (atoms235 + atoms238) * 6;

    CompMapPtr wComp = CompMapPtr(new CompMap(ATOM));
    (*wComp)[922350] = atoms235;
    (*wComp)[922380] = atoms238;
    (*wComp)[90190] = atoms19;

    //KDHFlag - Make sure you're not losing mass with this... you likely are. Think about it.
    mat_rsrc_ptr theTails = mat_rsrc_ptr(new Material(wComp));
    theTails->setQuantity(mat->quantity());

    // CONSERVATION OF MASS CHECKS:
    if (fabs(theProduct.mass(922350) +
          theTails.mass(922350) 
          - mat_iso.mass(922350)) > EPS_KG)
      throw CycException("Conservation of mass violation at Enrichment!!");

    // Don't forget to decrement outstMF before sending.
    outstMF_ -= this->calcSWUs(P, xp, xf);

    mat_rsrc_ptr rsrc = boost::dynamic_pointer_cast<Material>(mess->resource());
    rsrc->setQuantity(theProd->quantity());
    mess->setResource(boost::dynamic_pointer_cast<Resource>(theProd));

    mess->approveTransfer();
    wastes_.push_back(theTails);

    curr ++;
  }

  ordersExecuting_.erase(time);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double EnrichmentFacility::calcSWUs(double massProdU, double xp, double xf, double xw) {
  double term1 = (2 * xp - 1) * log(xp / (1 - xp));
  double term2 = (2 * xw - 1) * log(xw / (1 - xw)) * (xp - xf) / (xf - xw);
  double term3 = (2 * xf - 1) * log(xf / (1 - xf)) * (xp - xw) / (xf - xw);

  return massProdU * (term1 + term2 - term3);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double EnrichmentFacility::calcSWUs(double massProdU, double xp, double xf) {
  return EnrichmentFacility::calcSWUs(massProdU, xp, xf, default_xw_);
}

/* ------------------- */ 


/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

extern "C" Model* constructEnrichmentFacility() {
  return new EnrichmentFacility();
}

/* ------------------- */ 

