// SinkFacility.cpp
// Implements the SinkFacility class
#include <iostream>
#include "Logger.h"

#include "SinkFacility.h"

#include "GenericResource.h"
#include "CycException.h"
#include "InputXML.h"
#include "MarketModel.h"

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
void SinkFacility::init(xmlNodePtr cur) {
  FacilityModel::init(cur);

  /// Sink facilities can have many input/output commodities
  /// move XML pointer to current model
  cur = XMLinput->get_xpath_element(cur,"model/SinkFacility");

  /// all facilities require commodities - possibly many
  std::string commod;
  xmlNodeSetPtr nodes = XMLinput->get_xpath_elements(cur,"incommodity");
  for (int i=0;i<nodes->nodeNr;i++) {
    commod = (const char*)(nodes->nodeTab[i]->children->content);
    in_commods_.push_back(commod);
  }

  // get monthly capacity
  capacity_ = strtod(XMLinput->get_xpath_content(cur,"capacity"), NULL);

  // get inventory_size_
  inventory_size_ = strtod(XMLinput->get_xpath_content(cur,"inventorysize"), NULL);

  // get commodity price
  commod_price_ = strtod(XMLinput->get_xpath_content(cur,"commodprice"), NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void SinkFacility::copy(SinkFacility* src) {
  FacilityModel::copy(src);

  in_commods_ = src->in_commods_;
  capacity_ = src->capacity_;
  inventory_size_ = src->inventory_size_;
  commod_price_ = src->commod_price_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void SinkFacility::copyFreshModel(Model* src) {
  copy(dynamic_cast<SinkFacility*>(src));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void SinkFacility::print() {
  FacilityModel::print();

  std::string msg = "";

  msg += "accepts commodities ";

  for (vector<std::string>::iterator commod=in_commods_.begin();
       commod != in_commods_.end();
       commod++)
  {
    msg += (commod == in_commods_.begin() ? "{" : ", " );
    msg += (*commod);
  }
  msg += "} until its inventory is full at ";
  LOG(LEV_DEBUG2) << msg << inventory_size_ << " kg.";
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void SinkFacility::handleTick(int time){

  double requestAmt = getRequestAmt(); 
  double minAmt = 0;

  if (requestAmt>EPS_KG){
    // for each potential commodity, make a request
    for (vector<std::string>::iterator commod = in_commods_.begin();
        commod != in_commods_.end();
        commod++) {
      MarketModel* market = MarketModel::marketForCommod(*commod);
      Communicator* recipient = dynamic_cast<Communicator*>(market);

      // create a generic resource
      GenericResource* request_res = new GenericResource((*commod), "kg", requestAmt);

      // build the transaction and message
      Transaction trans;
      trans.commod = *commod;
      trans.minfrac = minAmt/requestAmt;
      trans.is_offer = false;
      trans.price = commod_price_;
      trans.resource = request_res;

      msg_ptr request = new Message(this, recipient, trans); 
      request->setNextDest(facInst());
      request->sendOn();

      LOG(LEV_DEBUG2) << "During handleTick, " << facName() << " requests: "<< requestAmt << ".";
    }
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void SinkFacility::handleTock(int time){
  // On the tock, the sink facility doesn't really do much. 
  // Maybe someday it will record things.
  // For now, lets just print out what we have at each timestep.
  LOG(LEV_DEBUG2) << "SinkFacility " << this->ID()
                  << " is holding " << this->checkInventory()
                  << " units of material at the close of month " << time
                  << ".";

  // call the facility model's handle tock last 
  // to check for decommissioning
  FacilityModel::handleTock(time);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void SinkFacility::addResource(msg_ptr msg, vector<Resource*> manifest) {
  
  // grab each material object off of the manifest
  // and move it into the inventory.
  for (vector<Resource*>::iterator thisMat=manifest.begin();
       thisMat != manifest.end();
       thisMat++) {
    LOG(LEV_DEBUG2) <<"SinkFacility " << ID() << " is receiving material with mass "
        << (*thisMat)->quantity();
    (*thisMat)->print();
    inventory_.push_back(dynamic_cast<Material*>(*thisMat));
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
double SinkFacility::checkInventory() {
  double total = 0;

  // Iterate through the inventory and sum the amount of whatever
  // material unit is in each object.

  deque<Material*>::iterator iter;

  for (iter = inventory_.begin(); iter != inventory_.end(); iter ++)
    total += (*iter)->quantity();

  return total;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
const double SinkFacility::getRequestAmt(){
  // The sink facility should ask for as much stuff as it can reasonably receive.
  double requestAmt;
  // get current capacity
  double emptiness = (inventory_size_- this->checkInventory());

  if (emptiness == 0 || emptiness < 0 ){
    requestAmt=0;
  } else if (emptiness < capacity_){
    requestAmt = emptiness/in_commods_.size();
  } else if (emptiness >= capacity_){
    requestAmt = capacity_/in_commods_.size();
  }
  return requestAmt;
}

/* --------------------
   output database info
 * --------------------
 */
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
std::string SinkFacility::outputDir_ = "/sink";

/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

extern "C" Model* constructSinkFacility() {
  return new SinkFacility();
}

extern "C" void destructSinkFacility(Model* p) {
  delete p;
}

/* ------------------- */ 

