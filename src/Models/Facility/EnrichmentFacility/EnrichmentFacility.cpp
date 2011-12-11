// EnrichmentFacility.cpp
// Implements the EnrichmentFacility class
#include <iostream>
#include "Logger.h"
#include <deque>

#include "EnrichmentFacility.h"

#include "Timer.h"
#include "Logician.h"
#include "CycException.h"
#include "InputXML.h"
#include "MarketModel.h"

/*
 * TICK
 * If there are ordersWaiting, prepare and send an appropriate 
 * request for raw material.
 * If there is capacity to produce any extra material next month
 * prepare and send an appropriate offer of SWUs.
 *
 * TOCK
 * Process as much raw stock material as capacity will allow.
 * Send appropriate materials to fill ordersWaiting.
 *
 *
 * RECIEVE MATERIAL
 * put it in stocks
 *
 * SEND MATERIAL
 * pull it from inventory
 * decrement ordersWaiting
 */

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void EnrichmentFacility::init(xmlNodePtr cur)
{ 
  FacilityModel::init(cur);
  
  // move XML pointer to current model
  cur = XMLinput->get_xpath_element(cur,"model/EnrichmentFacility");

  // all facilities require commodities - possibly many
  in_commod_ = XMLinput->get_xpath_content(cur,"incommodity");
  out_commod_ = XMLinput->get_xpath_content(cur,"outcommodity");

  // get inventory size
  inventory_size = strtod(XMLinput->get_xpath_content(cur,"inventorysize"), NULL);
  // get capacity_
  capacity_ = strtod(XMLinput->get_xpath_content(cur,"capacity"), NULL);
  // get default tails fraction
  default_xw_ = strtod(XMLinput->get_xpath_content(cur,"tailsassay"), NULL);

  inventory_ = deque<Material*>();
  stocks_ = deque<Material*>();
  ordersWaiting_ = deque<Message*>();
  ordersExecuting_ = ProcessLine();

  outstMF_ = 0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void EnrichmentFacility::copy(EnrichmentFacility* src)
{

  FacilityModel::copy(src);

  in_commod_ = src->in_commod_;
  out_commod_ = src->out_commod_;
  inventory_size = src->inventory_size;
  capacity_ = src->capacity_;
  default_xw_ = src->default_xw_;

  inventory_ = deque<Material*>();
  stocks_ = deque<Material*>();
  ordersWaiting_ = deque<Message*>();
  ordersExecuting_ = ProcessLine();

  outstMF_ = 0;
}



//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void EnrichmentFacility::copyFreshModel(Model* src)
{
  copy(dynamic_cast<EnrichmentFacility*>(src));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void EnrichmentFacility::print() 
{ 
  FacilityModel::print(); 
  LOG(LEV_DEBUG2) << "    converts commodity {"
      << in_commod_
      << "} into commodity {"
      << out_commod_
      << "}, and has an inventory that holds " 
      << inventory_size << " materials";
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void EnrichmentFacility::receiveMessage(Message* msg)
{
  // is this a message from on high? 
  if(msg->getSupplier()==this){
    // file the order
    ordersWaiting_.push_front(msg);
  }
  else {
    throw CycException("EnrichmentFacility is not the supplier of this msg.");
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void EnrichmentFacility::sendMaterial(Message* msg, const Communicator* requester)
{
  Transaction trans = msg->getTrans();
  // it should be of out_commod_ Commodity type
  if(trans.commod != out_commod_){
    throw CycException("EnrichmentFacility can only send '" +  out_commod_ + 
                       "' materials.");
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
    LOG(LEV_DEBUG2) <<"EnrichmentFacility "<< ID()
      <<"  is sending a mat with mass: "<< newMat->getTotMass();
  }    
  FacilityModel::sendMaterial(msg, toSend);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void EnrichmentFacility::receiveMaterial(Transaction trans, vector<Material*> manifest)
{
  // grab each material object off of the manifest
  // and move it into the stocks.
  for (vector<Material*>::iterator thisMat=manifest.begin();
       thisMat != manifest.end();
       thisMat++)
  {
    LOG(LEV_DEBUG2) <<"EnrichmentFacility " << ID() << " is receiving material with mass "
        << (*thisMat)->getTotMass();
    stocks_.push_back(*thisMat);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void EnrichmentFacility::handleTick(int time)
{
  // PROCESS ORDERS EXECUTING
  enrich();

  // MAKE REQUESTS
  makeRequests();

  // MAKE OFFERS
  makeOffers();

}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void EnrichmentFacility::handleTock(int time)
{
  // at rate allowed by capacity_, convert material in Stocks to out_commod_ type
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

  // fill the orders that are waiting, 
  while(!ordersWaiting_.empty()){
    Message* order = ordersWaiting_.front();
    sendMaterial(order, dynamic_cast<Communicator*>(order->getRequester()));
    ordersWaiting_.pop_front();
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
Mass EnrichmentFacility::checkInventory(){
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
Mass EnrichmentFacility::checkStocks(){
  Mass total = 0;

  // Iterate through the stocks and sum the amount of whatever
  // material unit is in each object.


  for (deque<Material*>::iterator iter = stocks_.begin(); 
       iter != stocks_.end(); 
       iter ++){
    total += (*iter)->getTotMass();
  }

  return total;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void EnrichmentFacility::makeRequests(){
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

  // this will be a request for free stuff
  // until cyclus has a working notion of default pricing
  // It would be nice to incorporate eqn 3.18 of Cochran & Tsoulfanidis
  // "The Nuclear Fuel Cycle : Analysis and Management" 
  double commod_price = 0;
  
  // spotCapacity represents unaccounted for capacity_
  Mass spotCapacity = capacity_ - outstMF_;

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
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void EnrichmentFacility::makeOffers()
{
  // decide how much to offer
  Mass offer_amt;
  Mass spotCapacity = capacity_ - outstMF_;

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

  // build the transaction and message
  Transaction trans;
  trans.commod = out_commod_;
  trans.min = min_amt;
  trans.price = commod_price;
  trans.amount = offer_amt; // offers have a positive amount

  Message* msg = new Message(this, recipient, trans); 
  msg->setNextDest(getFacInst());
  msg->sendOn();
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void EnrichmentFacility::enrich()
{
  // Get iterators that define the boundaries of the ordersExecuting that are 
  // currently ready.~
  int time = TI->getTime();

  pair<ProcessLine::iterator,
    ProcessLine::iterator> iters;
  iters = ordersExecuting_.equal_range(time);

  ProcessLine::iterator curr, omega;
  curr = iters.first;
  omega = iters.second;

  // Create and send Materials corresponding to each order that's ready to go.
  while (curr != omega) {

    // Get the info we need to make the enriched Material.
    Message* mess = (curr->second).first;
    Material* mat = (curr->second).second;

    // Find out what we're trying to make.
    //
    map<Iso, Atoms> compToMake; 
    try{
      compToMake = dynamic_cast<Material*>(mess->getResource())->getAtomComp();
    } catch (exception& e) {
      string err = "The Enrichment Facility may only receive a Material-type Resource";
      throw CycException(err);
    }

    // Do the enrichment math.
    double P = Material::getEltMass(92, compToMake);
    double xp = Material::getIsoMass(922350, compToMake) / P;
    double F = mat->getEltMass(92);
    double xf = Material::getIsoMass(922350, mat->getAtomComp()) / F;
    double W = F - P;
    double xw = (F * xf - P * xp) / W;

    // Make the product
    CompMap pComp;
    Atoms atoms235;
    Atoms atoms238;
    Atoms atoms19;
    Mass grams92;

    // in this moment, we assume that P is in tons... KDHFLAG
    grams92 = P * 1E6;
    atoms235 = grams92 * xp * AVOGADRO / 235;
    atoms238 = grams92 * (1 - xp) * AVOGADRO / 238;

    // The stoich for this one's easy:
    atoms19 = (atoms235 + atoms238) * 6;

    pComp[922350] = atoms235;
    pComp[922380] = atoms238;
    pComp[ 90190] = atoms19;

    string pName = string("eUF6%f",xp);
    Material* theProd = new Material(pComp, 
        mat->getUnits(), 
        pName,
        mat->getTotAtoms(), 
        ATOMBASED, 
        false);

    // Make the tails
    CompMap wComp;

    // in this moment, we assume that P is in tons... KDHFLAG
    grams92 = W * 1E6;
    atoms235 = grams92 * xw * AVOGADRO / 235;
    atoms238 = grams92 * (1 - xw) * AVOGADRO / 238;

    // The stoich for this one's easy:
    atoms19 = (atoms235 + atoms238) * 6;

    wComp[922350] = atoms235;
    wComp[922380] = atoms238;
    wComp[ 90190] = atoms19;

    //KDHFlag - Make sure you're not losing mass with this... you likely are. Think about it.
    string wName = string("dUF6%f",xw);
    Material* theTails = new Material(wComp, 
        mat->getUnits(),
        wName, 
        mat->getTotAtoms(), 
        ATOMBASED,
        false);

    // CONSERVATION OF MASS CHECKS:
    if (fabs(theProd->getEltMass(92) + theTails->getEltMass(92) 
          - mat->getEltMass(92)) > EPS_KG)
      throw CycException("Conservation of mass violation at Enrichment!!");

    if (fabs(Material::getIsoMass(922350, theProd->getAtomComp()) +
          Material::getIsoMass(922350, theTails->getAtomComp()) 
          - Material::getIsoMass(922350, mat->getAtomComp())) > EPS_KG)
      throw CycException("Conservation of mass violation at Enrichment!!");

    // Don't forget to decrement outstMF before sending.
    outstMF_ -= this->calcSWUs(P, xp, xf);

    mess->setAmount(theProd->getTotMass());
    mess->setResource(dynamic_cast<Resource*>(theProd));

    this->sendMaterial(mess, mess->getSender());
    wastes_.push_back(theTails);

    delete mat;
    curr ++;
  }

  ordersExecuting_.erase(time);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double EnrichmentFacility::calcSWUs(double massProdU, double xp, double xf, double xw)
{
  double term1 = (2 * xp - 1) * log(xp / (1 - xp));
  double term2 = (2 * xw - 1) * log(xw / (1 - xw)) * (xp - xf) / (xf - xw);
  double term3 = (2 * xf - 1) * log(xf / (1 - xf)) * (xp - xw) / (xf - xw);

  return massProdU * (term1 + term2 - term3);
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double EnrichmentFacility::calcSWUs(double massProdU, double xp, double xf)
{
  return EnrichmentFacility::calcSWUs(massProdU, xp, xf, default_xw_);
}

/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

extern "C" Model* construct() {
  return new EnrichmentFacility();
}

extern "C" void destruct(Model* p) {
  delete p;
}

/* ------------------- */ 

