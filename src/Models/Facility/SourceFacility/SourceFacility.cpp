// SourceFacility.cpp
// Implements the SourceFacility class
#include <iostream>

#include "SourceFacility.h"

#include "Logger.h"
#include "GenericResource.h"
#include "CycException.h"
#include "InputXML.h"
#include "MarketModel.h"
#include "Timer.h"

using namespace std;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
SourceFacility::SourceFacility() {
  prev_time_ = TI->time() - 1;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
SourceFacility::~SourceFacility() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void SourceFacility::init(xmlNodePtr cur) {
  FacilityModel::init(cur);

  LOG(LEV_DEBUG2, "SrcFac") << "A Source Facility is being initialized";

  /// move XML pointer to current model
  cur = XMLinput->get_xpath_element(cur,"model/SourceFacility");


  /// all facilities require commodities - possibly many
  string input_token;

  out_commod_ = XMLinput->get_xpath_content(cur,"outcommodity");

  // get recipe
  recipe_name_ = XMLinput->get_xpath_content(cur,"recipe");
  recipe_ = IsoVector::recipe(recipe_name_);

  // get capacity
  capacity_ = strtod(XMLinput->get_xpath_content(cur,"capacity"), NULL);

  // get inventory size
  double inv_size = strtod(XMLinput->get_xpath_content(cur,"inventorysize"), NULL);
  inventory_.setCapacity(inv_size);

  // get commodity price 
  commod_price_ = strtod(XMLinput->get_xpath_content(cur,"commodprice"), NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void SourceFacility::copy(SourceFacility* src) {
  FacilityModel::copy(src);

  out_commod_ = src->out_commod_;
  recipe_ = src->recipe_;
  capacity_ = src->capacity_;
  inventory_.setCapacity(src->inventory_.capacity());
  commod_price_ = src->commod_price_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void SourceFacility::copyFreshModel(Model* src) {
  copy( dynamic_cast<SourceFacility*>(src) );
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void SourceFacility::print() {
  FacilityModel::print();

  LOG(LEV_DEBUG2, "SrcFac!") << "    supplies commodity {"
      << out_commod_ << "} with recipe '" 
      << recipe_name_ << "' at a capacity of "
      << capacity_ << " kg per time step."
      << " It has a max inventory of " << inventory_.capacity() << " kg.";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void SourceFacility::receiveMessage(msg_ptr msg){

  // is this a message from on high? 
  if(msg->supplier() == this){
    // file the order
    ordersWaiting_.push_front(msg);
  } else {
    throw CycException("SourceFacility is not the supplier of this msg.");
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
vector<rsrc_ptr> SourceFacility::removeResource(msg_ptr msg) {
  Transaction trans = msg->trans();
  return ResourceBuff::toRes(inventory_.popQty(trans.resource->quantity()));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void SourceFacility::handleTick(int time){
  LOG(LEV_INFO3, "SrcFac") << facName() << " is ticking {";

  generateMaterial(time);
  Transaction trans = buildTransaction();

  LOG(LEV_INFO4, "SrcFac") << "offers "<< trans.resource->quantity() << " kg of "
                           << out_commod_ << ".";

  sendOffer(trans);

  LOG(LEV_INFO3, "SrcFac") << "}";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void SourceFacility::generateMaterial(int curr_time) {
  int time_change = curr_time - prev_time_;
  prev_time_ = curr_time;

  // if there's room in the inventory, process material at capacity
  double empty_space = inventory_.space();
  if (empty_space < EPS_KG) {return;}

  IsoVector temp = recipe_;
  if (capacity_ * recipe_.mass() * time_change <= empty_space) {
    // add a material the size of the capacity to the inventory
    temp.multBy(capacity_ * time_change);
  } else {
    // add a material that fills the inventory
    temp.setMass(empty_space);
  }
  mat_rsrc_ptr newMat = mat_rsrc_ptr(new Material(temp));
  newMat->setOriginatorID( this->ID() );
  inventory_.pushOne(newMat);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
Transaction SourceFacility::buildTransaction() {
  // there is no minimum amount a source facility may send
  double min_amt = 0;
  double offer_amt = inventory_.quantity();

  gen_rsrc_ptr offer_res = gen_rsrc_ptr(new GenericResource(out_commod_,"kg",offer_amt));

  Transaction trans;
  trans.commod = out_commod_;
  trans.minfrac = min_amt/offer_amt;
  trans.is_offer = true;
  trans.price = commod_price_;
  trans.resource = offer_res;

  return trans;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void SourceFacility::sendOffer(Transaction trans) {
  MarketModel* market = MarketModel::marketForCommod(out_commod_);

  Communicator* recipient = dynamic_cast<Communicator*>(market);
  msg_ptr msg(new Message(this, recipient, trans)); 
  msg->setNextDest(dynamic_cast<Communicator*>(parent()));
  msg->sendOn();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void SourceFacility::handleTock(int time){
  LOG(LEV_INFO3, "SrcFac") << facName() << " is tocking {";

  // check what orders are waiting,
  // send material if you have it now
  while (!ordersWaiting_.empty()) {
    msg_ptr order = ordersWaiting_.front();
    if (order->resource()->quantity() - inventory_.quantity() > EPS_KG) {
      LOG(LEV_INFO3, "SrcFac") << "Not enough inventory. Waitlisting remaining orders.";
      break;
    } else {
      order->approveTransfer();
      ordersWaiting_.pop_front();
    }
  }
  // For now, lets just print out what we have at each timestep.
  LOG(LEV_INFO4, "SrcFac") << "SourceFacility " << this->ID()
                  << " is holding " << this->inventory_.quantity()
                  << " units of material at the close of month " << time
                  << ".";

  LOG(LEV_INFO3, "SrcFac") << "}";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
extern "C" Model* constructSourceFacility() {
  return new SourceFacility();
}

