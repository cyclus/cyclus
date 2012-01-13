// SourceFacility.cpp
// Implements the SourceFacility class
#include <iostream>
#include "Logger.h"

#include "SourceFacility.h"
#include "GenericResource.h"
#include "CycException.h"
#include "InputXML.h"
#include "MarketModel.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
SourceFacility::SourceFacility() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
SourceFacility::~SourceFacility() {
  // Delete all the Material in the inventory.
  while (!inventory_.empty()) {
    Material* m = inventory_.front();
    inventory_.pop_front();
    delete m;
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void SourceFacility::init(xmlNodePtr cur) {
  FacilityModel::init(cur);

  LOG(LEV_DEBUG2) <<"The Source Facility is being initialized";

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

  // get inventory_size_
  inventory_size_ = strtod(XMLinput->get_xpath_content(cur,"inventorysize"), NULL);

  // get commodity price 
  commod_price_ = strtod(XMLinput->get_xpath_content(cur,"commodprice"), NULL);

  inventory_ = deque<Material*>();
  ordersWaiting_ = deque<Message*>();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void SourceFacility::copy(SourceFacility* src) {
  FacilityModel::copy(src);

  out_commod_ = src->out_commod_;
  recipe_ = src->recipe_;
  capacity_ = src->capacity_;
  inventory_size_ = src->inventory_size_;
  commod_price_ = src->commod_price_;
  
  inventory_ = deque<Material*>();
  ordersWaiting_ = deque<Message*>();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void SourceFacility::copyFreshModel(Model* src) {
  copy( dynamic_cast<SourceFacility*>(src) );
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void SourceFacility::print() {
  FacilityModel::print();

  LOG(LEV_DEBUG2) << "    supplies commodity {"
      << out_commod_ << "} with recipe '" 
      << recipe_name_ << "' at a capacity of "
      << capacity_ << " kg per time step."
      << " It has a max inventory of " << inventory_size_ << " kg.";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void SourceFacility::receiveMessage(Message* msg){

  // is this a message from on high? 
  if(msg->getSupplier()==this){
    // file the order
    ordersWaiting_.push_front(msg);
  }
  else {
    throw CycException("SourceFacility is not the supplier of this msg.");
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
vector<Resource*> SourceFacility::removeResource(Message* msg) {
  Transaction trans = msg->getTrans();
  double newAmt = 0;

  // pull materials off of the inventory stack until you get the trans amount

  // start with an empty manifest
  vector<Resource*> toSend;

  while (trans.resource->getQuantity() > newAmt && !inventory_.empty() ) {

    Material* m = inventory_.front();
    // if the inventory obj isn't larger than the remaining need, send it as is.
    if (m->getQuantity() <= (trans.resource->getQuantity() - newAmt)) {
      newAmt += m->getQuantity();
      toSend.push_back(m);
      inventory_.pop_front();
      LOG(LEV_DEBUG2) <<"SourceFacility "<< ID()
        <<"  has decided not to split the object with size :  "<< m->getQuantity();
    } else { 
      // if the inventory obj is larger than the remaining need, split it.
      Material* leftover = m->extract(trans.resource->getQuantity() - newAmt);
      newAmt += m->getQuantity();
      LOG(LEV_DEBUG2) <<"SourceFacility "<< ID()
        <<"  has decided to split the object to size :  "<< m->getQuantity();
      toSend.push_back(m);
      inventory_.pop_front();
      inventory_.push_back(leftover);
    }

    LOG(LEV_DEBUG2) <<"SourceFacility "<< ID()
      <<"  is sending a mat with mass: "<< m->getQuantity();
    (m)->print();
  }    
  return toSend;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void SourceFacility::handleTick(int time){
  // make offers
  // decide how much to offer
  double offer_amt;
  double inv = this->checkInventory();
  double possInv = inv + capacity_ * recipe_.mass(); 

  if (possInv < inventory_size_ * recipe_.mass()) {
    offer_amt = possInv;
  } else {
    offer_amt = inventory_size_* recipe_.mass(); 
  }

  // there is no minimum amount a source facility may send
  double min_amt = 0;

  // decide what market to offer to
  MarketModel* market = MarketModel::marketForCommod(out_commod_);
  Communicator* recipient = dynamic_cast<Communicator*>(market);
  LOG(LEV_DEBUG2) << "During handleTick, " << getFacName() << " offers: "<< offer_amt << ".";

  // build a generic resource to offer
  GenericResource* offer_res = new GenericResource(out_commod_,"kg",offer_amt);

  // build the transaction and message
  Transaction trans;
  trans.commod = out_commod_;
  trans.minfrac = min_amt/offer_amt;
  trans.is_offer = true;
  trans.price = commod_price_;
  trans.resource = offer_res;

  Message* msg = new Message(this, recipient, trans); 
  msg->setNextDest(getFacInst());
  msg->sendOn();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void SourceFacility::handleTock(int time){

  // if there's room in the inventory, process material at capacity
  double space = inventory_size_ - this->checkInventory(); 
  if (capacity_ * recipe_.mass() <= space) {
    // add a material the size of the capacity to the inventory
    IsoVector temp = recipe_;
    temp.multBy(capacity_);
    Material* newMat = new Material(temp);

    LOG(LEV_DEBUG2) << getFacName() << ", handling the tock, has created a material:";
    newMat->print();
    inventory_.push_front(newMat);
  } else if (space < capacity_*recipe_.mass() && space > 0) {
    // add a material that fills the inventory
    IsoVector temp = recipe_;
    temp.setMass(space);
    Material* newMat = new Material(temp);
    LOG(LEV_DEBUG2) << getFacName() << ", handling the tock, has created a material:";
    newMat->print();
    inventory_.push_front(newMat);
  }
  // check what orders are waiting,
  // send material if you have it now
  while (!ordersWaiting_.empty()) {
    Message* order = ordersWaiting_.front();
    order->approveTransfer();
    ordersWaiting_.pop_front();
  }
  // For now, lets just print out what we have at each timestep.
  LOG(LEV_DEBUG2) << "SourceFacility " << this->ID()
                  << " is holding " << this->checkInventory()
                  << " units of material at the close of month " << time
                  << ".";

  // call the facility model's handle tock last 
  // to check for decommissioning
  FacilityModel::handleTock(time);
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
double SourceFacility::checkInventory(){
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

/* --------------------
   output database info
 * --------------------
 */
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
std::string SourceFacility::outputDir_ = "/source";

/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

extern "C" Model* constructSourceFacility() {
  return new SourceFacility();
}

extern "C" void destructSourceFacility(Model* p) {
  delete p;
}

/* ------------------- */ 

