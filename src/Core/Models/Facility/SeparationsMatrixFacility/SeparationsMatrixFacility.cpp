// SeparationsMatrixFacility.cpp
// Implements the SeparationsMatrixFacility class
#include <iostream>
#include <sstream>
#include <deque>
#include <string.h>
#include <vector>

#include "SeparationsMatrixFacility.h"

#include "Logger.h"
#include "GenericResource.h"
#include "Timer.h"
#include "CycException.h"
#include "InputXML.h"

using namespace std;

/**
  TICK
  Make offers of separated material based on availabe inventory.
  If there are ordersWaiting, prepare and send an appropriate 
  request for spent fuel material.
  Check stocks to determine if there is capacity to produce any extra material
  next month. If so, process as much raw (spent fuel) stock material as
  capacity will allow.
 
  TOCK
  Send appropriate separated material from inventory to fill ordersWaiting.
 
  RECIEVE MATERIAL
  Put incoming spent nuclear fuel (SNF) material into stocks
 
  SEND MATERIAL
  Pull separated material from inventory based on Requests
  Decrement ordersWaiting
 */

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void SeparationsMatrixFacility::init(xmlNodePtr cur)
{
  FacilityModel::init(cur);

  /// move XML pointer to current model
  cur = XMLinput->get_xpath_element(cur,"model/SeparationsMatrixFacility");
  /// initialize any SeparationsMatrixFacility-specific datamembers here

  // all facilities require commodities - possibly many
  string new_commod;

  // Hack Force!!
  // Forcing Separations Matrix to know ahead of time the number of streams to be processed.
  int nodeForce_ = 1;

  // get incommodities
  xmlNodeSetPtr nodes = XMLinput->get_xpath_elements(cur,"incommodity");

  for (int i=0;i<nodes->nodeNr;i++)
  {
    xmlNodePtr commod = nodes->nodeTab[i];

    new_commod = XMLinput->get_xpath_content(cur,"incommodity");
    in_commod_.push_back(new_commod);
  }

  // get inventory size
  inventory_size_ = strtod(XMLinput->get_xpath_content(cur,"inventorysize"), NULL);

  // get capacity
  capacity_ = strtod(XMLinput->get_xpath_content(cur,"capacity"), NULL);

  // get stream
  nodes = XMLinput->get_xpath_elements(cur,"stream");
  // See nodeForce Hack Above

  for (int i=0;i<nodeForce_;i++)
  {

    xmlNodePtr stream = nodes->nodeTab[i];

    string new_commod = XMLinput->get_xpath_content(stream,"outcommodity");
    out_commod_.push_back(new_commod);

    int stream_Z = strtol(XMLinput->get_xpath_content(stream,"z"), NULL, 10);
    double stream_eff = strtod(XMLinput->get_xpath_content(stream,"eff"), NULL);
    stream_set_.insert(make_pair(new_commod, make_pair(stream_Z, stream_eff)));
    LOG(LEV_DEBUG2, "none!") << "Name = " << new_commod;
    LOG(LEV_DEBUG2, "none!") << "Z = " << stream_Z;
    LOG(LEV_DEBUG2, "none!") << "Eff = " << stream_eff;
  };

  inventory_ = deque<pair<string,mat_rsrc_ptr> >();
  stocks_ = deque<pair<string,mat_rsrc_ptr> >();
  ordersWaiting_ = deque<msg_ptr>();
  ordersExecuting_ = ProcessLine();

  outstMF_ = 0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void SeparationsMatrixFacility::copy(SeparationsMatrixFacility* src)
{
  FacilityModel::copy(src);

  in_commod_ = src->in_commod_;
  out_commod_ = src->out_commod_;
  inventory_size_ = src->inventory_size_;
  capacity_ = src->capacity_;

  inventory_ = deque<InSep>();
  stocks_ = deque<OutSep>();
  ordersWaiting_ = deque<msg_ptr>();
  ordersExecuting_ = ProcessLine();

  outstMF_ = 0;
}

// The Module below is added to match update from r240 to repository
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void SeparationsMatrixFacility::copyFreshModel(Model* src)
{
  copy(dynamic_cast<SeparationsMatrixFacility*>(src));
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
std::string SeparationsMatrixFacility::str() { 
  std::stringstream ss;
  ss << FacilityModel::str();

  ss << "converts commodities {";
  for(vector<string>::const_iterator iter = in_commod_.begin(); 
      iter != in_commod_.end(); 
      iter ++){
    ss << (*iter) << ", ";
  };

  ss << "} into commodities {";
  for (vector<string>::iterator iter = out_commod_.begin(); 
      iter != out_commod_.end(); 
      iter ++){
    ss << (*iter) << ", ";
  }; 
  ss << "}, with inventory holding " 
     << inventory_size_ << " materials";
  return ss.str();
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SeparationsMatrixFacility::receiveMessage(msg_ptr msg) 
{
  // is this a message from on high? 
  if(msg->trans().supplier()==this){
    // file the order
    ordersWaiting_.push_front(msg);
  }
  else {
    throw CycException("SeparationsMatrixFacility is not the supplier of this msg.");
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::vector<rsrc_ptr> SeparationsMatrixFacility::removeResource(Transaction order) {
  double newAmt = 0;

  // pull materials off of the inventory stack until you get the trans amount

  // start with an empty manifest
  vector<rsrc_ptr> toSend;

  while(order.resource()->quantity() > newAmt && !inventory_.empty() ){
    for (deque<InSep>::iterator iter = inventory_.begin(); 
        iter != inventory_.end(); 
        iter ++){
      mat_rsrc_ptr m = inventory_.front().second;

      // start with an empty material
      mat_rsrc_ptr newMat = mat_rsrc_ptr(new Material());

      // if the inventory obj isn't larger than the remaining need, send it as is.
      if(m->quantity() <= (order.resource()->quantity() - newAmt)){
        newAmt += m->quantity();
        newMat->absorb(m);
        inventory_.pop_front();
      }
      else{ 
        // if the inventory obj is larger than the remaining need, split it.
        mat_rsrc_ptr toAbsorb = m->extract(order.resource()->quantity() - newAmt);
        newAmt += toAbsorb->quantity();
        newMat->absorb(toAbsorb);
      }

      toSend.push_back(newMat);
      LOG(LEV_DEBUG2, "none!") <<"SeparationsMatrixFacility "<< ID()
        <<"  is sending a mat with mass: "<< newMat->quantity();
    }    
  }
  return toSend;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SeparationsMatrixFacility::addResource(Transaction trans,
                                            std::vector<rsrc_ptr> manifest) {  
  LOG(LEV_DEBUG2, "none!") << "Entered the addResource file ";

  // grab each material object off of the manifest
  // and move it into the stocks.
  for (vector<rsrc_ptr>::iterator thisMat=manifest.begin();
      thisMat != manifest.end();
      thisMat++) {
    LOG(LEV_DEBUG2, "none!") <<"SeparationsFacility " << ID() << " is receiving material with mass "
      << (*thisMat)->quantity();
    stocks_.push_back(make_pair(trans.commod(), boost::dynamic_pointer_cast<Material>(*thisMat)));
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SeparationsMatrixFacility::handleTick(int time) {
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
  // at rate allowed by capacity, convert material in Stocks to out_commod_ type
  // move converted material into Inventory


  // Handled more by separate function in handleTick than here."

  // Mass complete as in completely full inventory stored by SeparationsMatrix
  // Facility
  double complete = 0;

  while(capacity_ > complete && !stocks_.empty() ){
    for (deque<OutSep>::iterator iter = stocks_.begin(); 
        iter != stocks_.end(); 
        iter ++){
      mat_rsrc_ptr m = stocks_.front().second;

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
      // stocks.push_back(make_pair(trans.commod, *thisMat));
      // inventory_.push_back(make_pair(commod, newMat);
    }    

  } // <- for the for loop end


  // fill the orders that are waiting, 
  while(!ordersWaiting_.empty()) {
    msg_ptr order = ordersWaiting_.front();
    order->trans().approveTransfer();
    ordersWaiting_.pop_front();
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
double SeparationsMatrixFacility::checkInventory(){
  double total = 0;

  // Iterate through the inventory and sum the amount of whatever
  // material unit is in each object.

  for (deque<InSep>::iterator iter = inventory_.begin(); 
      iter != inventory_.end(); 
      iter ++){
    total += (*iter).second->quantity();
  }

  return total;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
double SeparationsMatrixFacility::checkStocks(){
  double total = 0;

  // Iterate through the stocks and sum the amount of whatever
  // material unit is in each object.


  for (deque<OutSep>::iterator iter = stocks_.begin(); 
      iter != stocks_.end(); 
      iter ++){
    total += (*iter).second->quantity();
  }

  return total;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void SeparationsMatrixFacility::makeRequests(){

  for (vector<string>::iterator iter = in_commod_.begin(); 
      iter != in_commod_.end(); 
      iter ++){
    // The separations facility should ask for at least as much SNF as it is 
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

    // Currently, no pricing information included for Separations Facility
    double commod_price = 0;

    // spotCapacity represents unaccounted for capacity
    double spotCapacity = capacity_ - outstMF_;

    if (space == 0){
      // don't request anything
    }
    else if (space < capacity_){
      int total = checkStocks();
      Communicator* recipient = dynamic_cast<Communicator*>(MarketModel::marketForCommod(*iter));
      // if empty space is less than monthly acceptance capacity
      requestAmt = space;

      // request a generic resource
      gen_rsrc_ptr request_res = gen_rsrc_ptr(new GenericResource((*iter), "kg", requestAmt));

      // build the transaction and message
      Transaction trans(this, REQUEST);
      trans.setCommod(*iter);
      trans.setMinFrac(minAmt/requestAmt);
      trans.setPrice(commod_price);
      trans.setResource(request_res);

      msg_ptr request(new Message(this, recipient, trans)); 
      request->sendOn();
    }
    // otherwise, the upper bound is the monthly acceptance capacity 
    // minus the amount in stocks.
    else if (space >= capacity_){
      Communicator* recipient = dynamic_cast<Communicator*>(MarketModel::marketForCommod(*iter));
      // if empty space is more than monthly acceptance capacity
      requestAmt = capacity_ - sto;

      // request a generic resource
      gen_rsrc_ptr request_res = gen_rsrc_ptr(new GenericResource((*iter), "kg", requestAmt));

      // build the transaction and message
      Transaction trans(this, REQUEST);
      trans.setCommod(*iter);
      trans.setMinFrac(minAmt/requestAmt);
      trans.setPrice(commod_price);
      trans.setResource(request_res);

      msg_ptr request(new Message(this, recipient, trans)); 
      request->sendOn();
    }

  } // <- for leop
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SeparationsMatrixFacility::makeOffers() {
  for (vector<string>::iterator iter = out_commod_.begin(); 
      iter != out_commod_.end(); 
      iter ++){
    // decide how much to offer
    double offer_amt;
    double spotCapacity = capacity_ - outstMF_;

    // and offer no more than the spotCapacity allows you to produce
    offer_amt = spotCapacity; 

    // there is no minimum amount a separations facility may send
    double min_amt = 0;

    // this will be an offer for free stuff
    // until cyclus has a working notion of default pricing for separated material
    double commod_price = 0;

    // decide what market to offer to
    Communicator* recipient = dynamic_cast<Communicator*>(MarketModel::marketForCommod(*iter));

    // build a material
    IsoVector comp;
    comp.setMass(1001, offer_amt);
    mat_rsrc_ptr offer_mat = mat_rsrc_ptr(new Material(comp));

    // build the transaction and message
    Transaction trans(this, OFFER);
    trans.setCommod(*iter);
    trans.setMinFrac(min_amt/offer_amt);
    trans.setPrice(commod_price);
    trans.setResource(offer_mat);

    msg_ptr msg(new Message(this, recipient, trans)); 
    msg->sendOn();
  }

}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SeparationsMatrixFacility::separate()
{
  // Get iterators that define the boundaries of the ordersExecuting_ that are 
  // currently ready.~
  int time = TI->time();

  pair<ProcessLine::iterator, ProcessLine::iterator> iters;

  iters = ordersExecuting_.equal_range(time);

  ProcessLine::iterator curr, omega;
  curr = iters.first;
  omega = iters.second;

  // Create and send Materials corresponding to each order that's ready to go.
  while (curr != omega) {
    // Get the info we need to make the separated Material.
    msg_ptr mess = (curr->second).first;
    mat_rsrc_ptr mat = (curr->second).second;

    // Find out what we're trying to make.
    try {
      IsoVector vecToMake =  boost::dynamic_pointer_cast<Material>(mess->trans().resource())->isoVector();
    } catch (exception& e) {
      string err = "The Resource sent to the SeparationsMatrixFacility \
                    must be a Material type Resource" ;
      throw CycException(err);
    }

    ordersExecuting_.erase(time);
  }

  /*
   * The section below is currently under development.  Its purpose is to do the
   * actual separations of the isotopes based on the string information.
   */


  // This loop will cycle through each element and then it will find out if 
  // there is anything in the stream that needs to be added to the material 
  //for separation.
  /*
     for(stream_set_.second.first::iterator iter = stream_set_.begin(); 
     !iter = stream_set_.end(); 
     iter++){            
     firstpair = inventory_.pop_front();
     string firstcommodity = firstpair.first();
     mat_rsrc_ptr firstmaterial = firstpair.second();
  // Multiply Amount of Element by Separation Efficieny and then add
  // it to the stock of material for that Element
  stocks_.second((*stream_set_).first) = 
  firstmaterial((*stream_set_).first)*((*stream_set_).second).second ++;

  }
  */       
}

/* ------------------- */ 


/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

extern "C" Model* constructSeparationsMatrixFacility() {
  return new SeparationsMatrixFacility();
}

/* ------------------- */ 


