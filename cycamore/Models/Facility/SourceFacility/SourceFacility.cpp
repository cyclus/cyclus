// SourceFacility.cpp
// Implements the SourceFacility class
#include <iostream>
#include <sstream>

#include "SourceFacility.h"

#include "Logger.h"
#include "RecipeLogger.h"
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
  recipe_ = RecipeLogger::Recipe(recipe_name_);

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
std::string SourceFacility::str() {
  std::stringstream ss;
  ss << FacilityModel::str()
     << " supplies commodity '"
     << out_commod_ << "' with recipe '" 
     << recipe_name_ << "' at a capacity of "
     << capacity_ << " kg per time step "
     << " with max inventory of " << inventory_.capacity() << " kg.";
  return "" + ss.str();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void SourceFacility::receiveMessage(msg_ptr msg){

  // is this a message from on high? 
  if(msg->trans().supplier() == this){
    // file the order
    ordersWaiting_.push_front(msg);
    LOG(LEV_INFO5, "SrcFac") << name() << " just received an order.";
  } else {
    throw CycException("SourceFacility is not the supplier of this msg.");
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
vector<rsrc_ptr> SourceFacility::removeResource(Transaction order) {
  return MatBuff::toRes(inventory_.popQty(order.resource()->quantity()));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void SourceFacility::handleTick(int time){
  LOG(LEV_INFO3, "SrcFac") << facName() << " is ticking {";

  generateMaterial(time);
  Transaction trans = buildTransaction();

  LOG(LEV_INFO4, "SrcFac") << "offers "<< trans.resource()->quantity() << " kg of "
                           << out_commod_ << ".";

  sendOffer(trans);

  LOG(LEV_INFO3, "SrcFac") << "}";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void SourceFacility::generateMaterial(int curr_time) {
  int time_change = curr_time - prev_time_;
  prev_time_ = curr_time;

  double empty_space = inventory_.space();
  if (empty_space < EPS_KG) {
    return; // no room
  }

  mat_rsrc_ptr newMat = mat_rsrc_ptr(new Material(recipe_));
  double amt = capacity_ * time_change;
  if (amt <= empty_space) {
    newMat->setQuantity(amt); // plenty of room
  } else {
    newMat->setQuantity(empty_space); // not enough room
  }
  inventory_.pushOne(newMat);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
Transaction SourceFacility::buildTransaction() {
  // there is no minimum amount a source facility may send
  double min_amt = 0;
  double offer_amt = inventory_.quantity();

  gen_rsrc_ptr offer_res = gen_rsrc_ptr(new GenericResource(out_commod_,"kg",offer_amt));

  Transaction trans(this, OFFER);
  trans.setCommod(out_commod_);
  trans.setMinFrac(min_amt/offer_amt);
  trans.setPrice(commod_price_);
  trans.setResource(offer_res);

  return trans;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void SourceFacility::sendOffer(Transaction trans) {
  MarketModel* market = MarketModel::marketForCommod(out_commod_);

  Communicator* recipient = dynamic_cast<Communicator*>(market);
  msg_ptr msg(new Message(this, recipient, trans)); 
  msg->sendOn();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void SourceFacility::handleTock(int time){
  LOG(LEV_INFO3, "SrcFac") << facName() << " is tocking {";

  // check what orders are waiting,
  // send material if you have it now
  while (!ordersWaiting_.empty()) {
    Transaction order = ordersWaiting_.front()->trans();
    if (order.resource()->quantity() - inventory_.quantity() > EPS_KG) {
      LOG(LEV_INFO3, "SrcFac") << "Not enough inventory. Waitlisting remaining orders.";
      break;
    } else {
      order.approveTransfer();
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

