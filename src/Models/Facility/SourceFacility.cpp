// SourceFacility.cpp
// Implements the SourceFacility class
#include <iostream>

#include "SourceFacility.h"
#include "Logician.h"
#include "CycException.h"
#include "InputXML.h"

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

  cout<<"The Source Facility is being initialized"<<endl;

  /// move XML pointer to current model
  cur = XMLinput->get_xpath_element(cur,"model/SourceFacility");


  /// all facilities require commodities - possibly many
  string input_token;

  input_token = XMLinput->get_xpath_content(cur,"outcommodity");
  out_commod_ = Commodity::getCommodity(input_token);

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

  cout << "supplies commodity {"
      << out_commod_->getName() << "} with recipe '" 
      << recipe_->getName() << "' at a capacity of "
      << capacity_ << " " << recipe_->getUnits() << " per time step."
      << " It has a max inventory of " << inventory_size_ << " " 
      << recipe_->getUnits() <<  "." << endl;
  
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

  while(trans.amount > newAmt && !inventory_.empty() ){
    // start with an empty material
    Material* newMat = new Material(CompMap(), 
                                  recipe_->getUnits(),
                                  recipe_->getName(), 
                                  0, MASSBASED);

    Material* m = inventory_.front();
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
    cout<<"SourceFacility "<< getSN()
      <<"  is sending a mat with mass: "<< newMat->getTotMass()<< endl;
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
  Communicator* recipient = dynamic_cast<Communicator*>(out_commod_->getMarket());
  cout << "During handleTick, " << getFacName() << " offers: "<< offer_amt << "."  << endl;

  // create a message to go up to the market with these parameters
  Message* msg = new Message(UP_MSG, out_commod_, offer_amt, min_amt, commod_price_, 
      this, recipient);

  // send it
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
                                    recipe_->getName(),
                                    capacity_*recipe_->getTotMass(), 
                                    MASSBASED);
    cout << getFacName() << ", handling the tock, has created a material:"<<endl;
    newMat->print();
    inventory_.push_front(newMat);
  }
  else if (space < capacity_*recipe_->getTotMass() && space > 0){
    // add a material that fills the inventory
    Material* newMat = new Material(recipe_->getMassComp(), 
                                    recipe_->getUnits(), 
                                    recipe_->getName(),
                                    space,
                                    ATOMBASED);
    cout << getFacName() << ", handling the tock, has created a material:"<<endl;
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
  cout << "SourceFacility " << this->getSN();
  cout << " is holding " << this->checkInventory();
  cout << " units of material at the close of month " << time; 
  cout << "." << endl;

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

