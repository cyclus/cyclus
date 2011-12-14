// SourceFacility.cpp
// Implements the SourceFacility class
#include <iostream>
#include "Logger.h"

#include "SourceFacility.h"
#include "Logician.h"
#include "CycException.h"
#include "InputXML.h"
#include "MarketModel.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
SourceFacility::SourceFacility(){ }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
SourceFacility::~SourceFacility(){
  // Delete all the Material in the inventory.
  while (!inventory_.empty()) {
    Material* m = inventory_.front();
    inventory_.pop_front();
    delete m;
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void SourceFacility::init(xmlNodePtr cur)
{
  FacilityModel::init(cur);

  LOG(LEV_DEBUG2) <<"The Source Facility is being initialized";

  /// move XML pointer to current model
  cur = XMLinput->get_xpath_element(cur,"model/SourceFacility");


  /// all facilities require commodities - possibly many
  string input_token;

  out_commod_ = XMLinput->get_xpath_content(cur,"outcommodity");

  // get recipe
  input_token = XMLinput->get_xpath_content(cur,"recipe");
  recipe_ = LI->getRecipe(input_token);

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
void SourceFacility::copy(SourceFacility* src)
{
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
void SourceFacility::copyFreshModel(Model* src)
{
  copy( dynamic_cast<SourceFacility*>(src) );
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void SourceFacility::print() 
{ 
  FacilityModel::print();

  LOG(LEV_DEBUG2) << "    supplies commodity {"
      << out_commod_ << "} with recipe '" 
      << recipe_->name() << "' at a capacity of "
      << capacity_ << " " << recipe_->getUnits() << " per time step."
      << " It has a max inventory of " << inventory_size_ << " " 
      << recipe_->getUnits() <<  ".";
  
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
void SourceFacility::sendMaterial(Message* msg, const Communicator* requester)
{
  Transaction trans = msg->getTrans();
  Mass newAmt = 0;

  // pull materials off of the inventory stack until you get the trans amount

  // start with an empty manifest
  vector<Material*> toSend;

  while(trans.resource->getQuantity() > newAmt && !inventory_.empty() ){
    // start with an empty material
    Material* newMat = new Material(CompMap(), 
                                  recipe_->getUnits(),
                                  recipe_->name(), 
                                  0, 
                                  MASSBASED,
                                  false);

    Material* m = inventory_.front();
    // if the inventory obj isn't larger than the remaining need, send it as is.
    if(m->getTotMass() <= (trans.resource->getQuantity() - newAmt)){
      newAmt += m->getTotMass();
      newMat->absorb(m);
      inventory_.pop_front();
    }
    else{ 
      // if the inventory obj is larger than the remaining need, split it.
      Material* toAbsorb = m->extractMass(trans.resource->getQuantity() - newAmt);
      newAmt += toAbsorb->getTotMass();
      newMat->absorb(toAbsorb);
    }

    toSend.push_back(newMat);
    LOG(LEV_DEBUG2) <<"SourceFacility "<< ID()
      <<"  is sending a mat with mass: "<< newMat->getTotMass();
    (newMat)->print();
  }    
  FacilityModel::sendMaterial(msg, toSend);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void SourceFacility::handleTick(int time){
  // make offers
  // decide how much to offer
  Mass offer_amt;
  Mass inv = this->checkInventory();
  Mass possInv = inv+capacity_*recipe_->getTotMass(); 

  if (possInv < inventory_size_*recipe_->getTotMass()){
    offer_amt = possInv;
  }
  else {
    offer_amt = inventory_size_*recipe_->getTotMass(); 
  }

  // there is no minimum amount a source facility may send
  double min_amt = 0;

  // decide what market to offer to
  MarketModel* market = MarketModel::marketForCommod(out_commod_);
  Communicator* recipient = dynamic_cast<Communicator*>(market);
  LOG(LEV_DEBUG2) << "During handleTick, " << getFacName() << " offers: "<< offer_amt << ".";

  // build a material to offer
  Material* offer_mat = new Material(CompMap(),"","",offer_amt,MASSBASED,true);

  // build the transaction and message
  Transaction trans;
  trans.commod = out_commod_;
  trans.minfrac = min_amt/offer_amt;
  trans.is_offer = true;
  trans.price = commod_price_;
  trans.resource = offer_mat;

  Message* msg = new Message(this, recipient, trans); 
  msg->setNextDest(getFacInst());
  msg->sendOn();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void SourceFacility::handleTock(int time){
  // if there's room in the inventory, process material at capacity
  Mass space = inventory_size_ - this->checkInventory(); 
  if(capacity_*recipe_->getTotMass() <= space){
    // add a material the size of the capacity to the inventory
    Material* newMat = new Material(recipe_->getMassComp(), 
                                    recipe_->getUnits(), 
                                    recipe_->name(),
                                    capacity_*recipe_->getTotMass(), 
                                    MASSBASED,
                                    false);
    LOG(LEV_DEBUG2) << getFacName() << ", handling the tock, has created a material:";
    newMat->print();
    inventory_.push_front(newMat);
  }
  else if (space < capacity_*recipe_->getTotMass() && space > 0){
    // add a material that fills the inventory
    Material* newMat = new Material(recipe_->getMassComp(), 
                                    recipe_->getUnits(), 
                                    recipe_->name(),
                                    space,
                                    ATOMBASED,
                                    false);
    LOG(LEV_DEBUG2) << getFacName() << ", handling the tock, has created a material:";
    newMat->print();
    inventory_.push_front(newMat);
  }
  // check what orders are waiting,
  // send material if you have it now
  while(!ordersWaiting_.empty()){
    Message* order = ordersWaiting_.front();
    sendMaterial(order, dynamic_cast<Communicator*>(order->getRequester()));
    ordersWaiting_.pop_front();
  }
  // Maybe someday it will record things.
  // For now, lets just print out what we have at each timestep.
  LOG(LEV_DEBUG2) << "SourceFacility " << this->ID()
                  << " is holding " << this->checkInventory()
                  << " units of material at the close of month " << time
                  << ".";

}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
Mass SourceFacility::checkInventory(){
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

/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

extern "C" Model* construct() {
  return new SourceFacility();
}

extern "C" void destruct(Model* p) {
  delete p;
}

/* ------------------- */ 

