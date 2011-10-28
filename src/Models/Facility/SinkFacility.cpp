// SinkFacility.cpp
// Implements the SinkFacility class
#include <iostream>

#include "SinkFacility.h"

#include "Logician.h"
#include "CycException.h"
#include "InputXML.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
SinkFacility::SinkFacility(){
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
SinkFacility::~SinkFacility(){
  // Delete all the Material in the inventory.
  while (!inventory_.empty()) {
    Material* m = inventory_.front();
    inventory_.pop_front();
    delete m;
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void SinkFacility::init(xmlNodePtr cur)
{
  FacilityModel::init(cur);

  /** 
   *  Allow a Sink Facility to have many input/output commodities
   */

  /// move XML pointer to current model
  cur = XMLinput->get_xpath_element(cur,"model/SinkFacility");

  /// all facilities require commodities - possibly many
  string commod_name;
  Commodity* new_commod;
  xmlNodeSetPtr nodes = XMLinput->get_xpath_elements(cur,"incommodity");

  for (int i=0;i<nodes->nodeNr;i++)
  {
    commod_name = (const char*)(nodes->nodeTab[i]->children->content);
    new_commod = Commodity::getCommodity(commod_name);
    in_commods_.push_back(new_commod);
  }

  // get monthly capacity
  capacity_ = strtod(XMLinput->get_xpath_content(cur,"capacity"), NULL);

  // get inventory_size_
  inventory_size_ = strtod(XMLinput->get_xpath_content(cur,"inventorysize"), NULL);

  // get commodity price
  commod_price_ = strtod(XMLinput->get_xpath_content(cur,"commodprice"), NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void SinkFacility::copy(SinkFacility* src)
{
  FacilityModel::copy(src);

  in_commods_ = src->in_commods_;
  capacity_ = src->capacity_;
  inventory_size_ = src->inventory_size_;
  commod_price_ = src->commod_price_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void SinkFacility::copyFreshModel(Model* src)
{
  copy(dynamic_cast<SinkFacility*>(src));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void SinkFacility::print() 
{ 
  FacilityModel::print();

  cout << "accepts commodities ";
  for (vector<Commodity*>::iterator commod=in_commods_.begin();
       commod != in_commods_.end();
       commod++)
  {
    cout << (commod == in_commods_.begin() ? "{" : ", " );
    cout << (*commod)->getName();
  }
  cout << "} until its inventory is full at ";
  cout << inventory_size_ << " units." << endl ;
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void SinkFacility::handleTick(int time){
  // The sink facility should ask for as much stuff as it can reasonably receive.
  Mass requestAmt;
  // And it can accept amounts no matter how small
  Mass minAmt = 0;
  // check how full its inventory is
  Mass fullness = this->checkInventory();
  // subtract from max size to get total empty space
  Mass emptiness = inventory_size_ - fullness;

  if (emptiness == 0){
    // don't request anything
  }
  else if (emptiness < capacity_){
  // if empty space is less than monthly acceptance capacity, request it,
    // for each commodity, request emptiness/(noCommodities)
    for (vector<Commodity*>::iterator commod=in_commods_.begin();
       commod != in_commods_.end();
       commod++) 
    {
      Communicator* recipient = dynamic_cast<Communicator*>((*commod)->getMarket());
      // recall that requests have a negative amount
      requestAmt = (emptiness/in_commods_.size());
      Message* request = new Message(UP_MSG, *commod, -requestAmt, minAmt, 
                                     commod_price_, this, recipient);
      // pass the message up to the inst
      request->setNextDest(getFacInst());
      request->sendOn();

      cout << "During handleTick, " << getFacName() << " requests: "<< requestAmt << "."  << endl;
    }
  }
  // otherwise, the upper bound is the monthly acceptance capacity, request cap.
  else if (emptiness >= capacity_){
    // for each commodity, request capacity/(noCommodities)
    for (vector<Commodity*>::iterator commod=in_commods_.begin();
       commod != in_commods_.end();
       commod++) 
    {
      Communicator* recipient = dynamic_cast<Communicator*>((*commod)->getMarket());
      requestAmt = capacity_/in_commods_.size();
      Message* request = new Message(UP_MSG, *commod, -requestAmt, minAmt, commod_price_,
                          this, recipient); 
    // pass the message up to the inst
    request->setNextDest(getFacInst());
    request->sendOn();

    cout << "During handleTick, " << getFacName() << " requests: " << requestAmt << "."  << endl;
    }
  }

}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void SinkFacility::handleTock(int time){

  // On the tock, the sink facility doesn't really do much. 
  // Maybe someday it will record things.
  // For now, lets just print out what we have at each timestep.
  cout << "SinkFacility " << this->getSN();
  cout << " is holding " << this->checkInventory();
  cout << " units of material at the close of month " << time; 
  cout << "." << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void SinkFacility::receiveMaterial(Transaction trans, vector<Material*> manifest){

  // grab each material object off of the manifest
  // and move it into the inventory.
  for (vector<Material*>::iterator thisMat=manifest.begin();
       thisMat != manifest.end();
       thisMat++)
  {
    cout<<"SinkFacility " << getSN() << " is receiving material with mass "
        << (*thisMat)->getTotMass() << endl;
    (*thisMat)->print();
    inventory_.push_back(*thisMat);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
Mass SinkFacility::checkInventory(){
  Mass total = 0;

  // Iterate through the inventory and sum the amount of whatever
  // material unit is in each object.

  deque<Material*>::iterator iter;

  for (iter = inventory_.begin(); iter != inventory_.end(); iter ++)
    total += (*iter)->getTotMass();

  return total;
}

/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

extern "C" Model* construct() {
  return new SinkFacility();
}

extern "C" void destruct(Model* p) {
  delete p;
}

/* ------------------- */ 

