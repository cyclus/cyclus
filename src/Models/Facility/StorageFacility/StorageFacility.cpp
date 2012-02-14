// StorageFacility.cpp
// Implements the StorageFacility class
#include <iostream>
#include "Logger.h"

#include "StorageFacility.h"

#include "CycException.h"
#include "InputXML.h"
#include "Timer.h"
#include "BookKeeper.h"

/*
 * TICK
 * send a request for your capacity minus your stocks.
 * offer materials that have exceeded their residence times
 * offer them
 *
 * TOCK
 * send appropriate materials to fill ordersWaiting.
 *
 * RECIEVE MATERIAL
 * put it in stocks
 * add it to the deque of release times
 *
 * SEND MATERIAL
 * pull it from inventory, fill the transaction
 */


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void StorageFacility::init(xmlNodePtr cur)
{ 
  FacilityModel::init(cur);

  incommod_ =  NULL; 
  
  // move XML pointer to current model
  cur = XMLinput->get_xpath_element(cur,"model/StorageFacility");

  // all facilities require commodities - possibly many
  string commod_name;
  Commodity* new_commod;
  
  commod_name = XMLinput->get_xpath_content(cur,"incommodity");
  incommod_ = Commodity::getCommodity(commod_name);
  
  inventory_size_ = strtod(XMLinput->get_xpath_content(cur,"inventorysize"), NULL);
  capacity_ = strtod(XMLinput->get_xpath_content(cur,"capacity"), NULL);
  residence_time_ = strtod(XMLinput->get_xpath_content(cur,"residencetime"), NULL);


  inventory_ = deque<mat_rsrc_ptr>();
  stocks_ = deque<mat_rsrc_ptr>();
  ordersWaiting_ = deque<msg_ptr>();
  
  initialStateCur_ = cur;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void StorageFacility::copy(StorageFacility* src)
{

  FacilityModel::copy(src);

  incommod_ = src->incommod_;
  inventory_size_ = src->inventory_size_;
  capacity_ = src->capacity_;
  residence_time_ = src->residence_time_;

  // do we really want all of these to be copied?
  inventory_ = src->inventory_;
  stocks_ = src->stocks_;
  ordersWaiting_ = src->ordersWaiting_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void StorageFacility::copyFreshModel(Model* src)
{
  copy(dynamic_cast<StorageFacility*>(src));
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void StorageFacility::print() 
{ 
  FacilityModel::print(); 
  LOG(LEV_DEBUG2) << "    stores commodity {"
      << incommod_->getName()
      << "}, for a minimum time of " 
      << residence_time_ 
      << " months and has an inventory_ that holds " 
      << inventory_size_ << " materials."
;
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void StorageFacility::receiveMessage(msg_ptr msg)
{
  // is this a message from on high? 
  if(msg->supplier()==this){
    // file the order
    ordersWaiting_.push_front(msg);
  }
  else {
    throw CycException("StorageFacility is not the supplier of this msg.");
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
std::vector<rsrc_ptr> StorageFacility::removeResource(msg_ptr order) {
  Transaction trans = order->trans();
  // it should be of incommod Commodity type
  if(trans.commod != incommod_){
    throw CycException("StorageFacility can only send incommodity type materials.");
  }
  // pull materials off of the inventory_ stack until you get the trans amount
  Mass complete = 0;

  // start with an empty manifest
  vector<rsrc_ptr> toSend;

  while(trans.amount > complete && !inventory_.empty() ){
    mat_rsrc_ptr m = inventory_.front();

    // if the inventory_ obj isn't larger than the remaining need, send it as is.
    if(m->quantity() <= (capacity_ - complete)){
      complete += m->quantity();
      toSend.push_back(m);
      LOG(LEV_DEBUG2) <<"StorageFacility "<< getSN()
        <<"  is sending a mat with mass: "<< m->quantity();
      inventory_.pop_front();
    } else { 
      // if the inventory_ obj is larger than the remaining need, split it.
      // start with an empty material
      mat_rsrc_ptr newMat = new Material(CompMap(), 
          m->getUnits(),
          m->getName(), 
          0, ATOMBASED);
      mat_rsrc_ptr toAbsorb = m->extractMass(capacity_ - complete);
      complete += toAbsorb->quantity();
      newMat->absorb(toAbsorb);
      toSend.push_back(newMat);
      LOG(LEV_DEBUG2) <<"StorageFacility "<< getSN()
        <<"  is sending a mat with mass: "<< newMat->quantity();
    }
  }    
  return toSend;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void StorageFacility::addResource(msg_ptr msg, vector<rsrc_ptr> manifest) {
  // grab each material object off of the manifest
  // and move it into the stocks.
  // also record its entry time map in entryTimes deque
  for (vector<rsrc_ptr>::iterator thisMat=manifest.begin();
       thisMat != manifest.end();
       thisMat++) {
    LOG(LEV_DEBUG2) <<"StorageFacility " << getSN() << " is receiving material with mass "
        << (*thisMat)->quantity();
    stocks_.push_back(dynamic_cast<mat_rsrc_ptr>(*thisMat));
    entryTimes_.push_back(make_pair(TI->time(), dynamic_cast<mat_rsrc_ptr>(*thisMat) ));
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void StorageFacility::getInitialState(xmlNodePtr cur)
 {
  xmlNodeSetPtr nodes = XMLinput->get_xpath_elements(cur, "initialstocks/entry");
  string fac_name, commod_name, recipe_name;
  FacilityModel* sending_facility;
  Commodity* commodity;
  mat_rsrc_ptr recipe;
  double amount, age;
  int i, nNodes = nodes->nodeNr;
  LOG(LEV_DEBUG2) << "**** nNodes = " << nNodes;

  // for each fuel pair, there is an in and an out commodity
  for (int i=0;i<nNodes;i++){
    // get xml node
    xmlNodePtr entry_node = nodes->nodeTab[i];

    // assign each item initially in storage
    sending_facility, commodity, recipe = NULL;
    // facility
    fac_name = XMLinput->get_xpath_content(entry_node,"facility");
    sending_facility = dynamic_cast<FacilityModel*>(Model::getModelByName(fac_name));

    // commodity
    commod_name = XMLinput->get_xpath_content(entry_node,"incommodity");
    commodity = Commodity::getCommodity(commod_name);

    // recipe
    recipe_name = XMLinput->get_xpath_content(entry_node,"recipe");
    recipe = IsoVector::recipe(recipe_name);

    // amount
    amount = strtod(XMLinput->get_xpath_content(entry_node,"amount"), NULL);
    // time in storage (age) in months
    age = strtod(XMLinput->get_xpath_content(entry_node,"age"), NULL);

    // make new material
    mat_rsrc_ptr newMat = new Material(recipe->getMassComp(), 
                                    recipe->getUnits(), 
                                    recipe->getName(),
                                    amount, 
                                    MASSBASED);
    
    // decay the material for the alloted time
    newMat->decay(age);

    vector <mat_rsrc_ptr> manifest;
    manifest.push_back(newMat);

    /* this needs to be fixed */
    // create the book keeping message
    double price = 0.0, minAmt = 0.0;

    // build the transaction and message
    Transaction trans;
    trans.commod = commodity;
    trans.comp = newMat->getAtomComp();
    trans.min = minAmt;
    trans.price = price;
    trans.amount = newMat->quantity();

    msg_ptr storage_history(new Message(sending_facility, this, trans); 
    storage_history->approveTransfer();
    sending_facility->sendMaterial(storage_history,manifest);
  }
  
  LOG(LEV_DEBUG2) << "\n ** Checking initial stocks of size " << stocks_.size() << " **\n";
  // check to make sure we got the correct initial inventory_
  for (int i=0;i<stocks_.size();i++){
    stocks_[i]->print();
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void StorageFacility::handlePreHistory()
{
  getInitialState( initialStateCur_ );
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void StorageFacility::handleTick(int time)
{

  // MAKE A REQUEST
  // The storage facility should ask for as much stuff as it can reasonably receive.
  Mass requestAmt;
  // And it can accept amounts no matter how small
  Mass minAmt = 0;
  // check how full its inventory_ is
  Mass inv = this->checkInventory();
  // and how much is already in its stocks
  Mass sto = this->checkStocks(); 
  LOG(LEV_DEBUG2) << "stocks currently at: " << sto << " " << inv;
  // subtract inv and sto from inventory_ max size to get total empty space
  Mass space = inventory_size_ - inv - sto;
  // this will be a request for free stuff
  double commod_price = 0;

  Communicator* recipient;
  recipient = dynamic_cast<Communicator*>(MarketModel::marketForCommod(incommod_));
  if (space == 0){
    // don't request anything
  } else if (space < capacity_){
    // if empty space is less than monthly acceptance capacity
    requestAmt = space;

    // build the transaction and message
    Transaction trans;
    trans.commod = in_commod_;
    trans.min = minAmt;
    trans.price = commod_price;
    trans.amount = -requestAmt; // requests have a negative amount

    msg_ptr request(new Message(this, recipient, trans)); 
    request->setNextDest(facInst());
    request->sendOn();

  // otherwise, the upper bound is the monthly acceptance capacity 
  // minus the amount in stocks.
  } else if (space >= capacity_){
    // if empty space is more than monthly acceptance capacity
    requestAmt = capacity_ - sto;

    // build the transaction and message
    Transaction trans;
    trans.commod = in_commod_;
    trans.min = minAmt;
    trans.price = commod_price;
    trans.amount = -requestAmt; // requests have a negative amount

    msg_ptr request(new Message(this, recipient, trans)); 
    request->setNextDest(facInst());
    request->sendOn();
  }
  
  // MAKE OFFERS
  // anything in the inventory_ is old enough to leave 
  Mass offer_amt;
  Mass possInv = inv;

  // if the inventory_ isn't full, then offer what you have
  if (possInv < inventory_size_){
    offer_amt = possInv;
  }
  else {
    offer_amt = inventory_size_; 
  }

  // there is no minimum amount a storage facility may send
  double min_amt = 0;

  // decide what market to offer to
  Communicator* recipient = dynamic_cast<Communicator*>(incommod_->market());

  // build the transaction and message
  Transaction trans;
  trans.commod = incommod_;
  trans.min = min_amt;
  trans.price = commod_price;
  trans.amount = offer_amt; // offers have a positive amount

  msg_ptr msg(new Message(this, recipient, trans)); 
  msg->setNextDest(facInst());
  msg->sendOn();
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void StorageFacility::handleTock(int time)
{
  // CHECK MATERIAL AGES
  // Some materials in the stocks have reached their minimum residence time. 
  // put them in the inventory_
  bool someOld = true;
  while( someOld == true && !stocks_.empty()){
    mat_rsrc_ptr oldEnough = stocks_.front();
    if(TI->time() - entryTimes_.front().first >= residence_time_ ){
        entryTimes_.pop_front();
        // Here is is where we could add a case switch between sending
        // youngest or oldest material first
        inventory_.push_back(oldEnough);
        stocks_.pop_front();
    }
    // added this 5/17/11 because an initial inventory would cause an infinite loop
    // not positive this is correct
    else{someOld=false;};
  };

  // check what orders are waiting, 
  while(!ordersWaiting_.empty()){
    msg_ptr order = ordersWaiting_.front();
    order->approveTransfer();
    ordersWaiting_.pop_front();
  }
  
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
Mass StorageFacility::checkInventory(){
  Mass total = 0;

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
Mass StorageFacility::checkStocks(){
  Mass total = 0;

  // Iterate through the stocks and sum the amount of whatever
  // material unit is in each object.


  for (deque<mat_rsrc_ptr>::iterator iter = stocks_.begin(); 
       iter != stocks_.end(); 
       iter ++){
    total += (*iter)->quantity();
  }

  return total;
}

/* --------------------
   output database info
 * --------------------
 */
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
std::string StorageFacility::outputDir_ = "/storage";

/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

extern "C" Model* constructStorageFacility() {
    return new StorageFacility();
}


/* ------------------- */ 

