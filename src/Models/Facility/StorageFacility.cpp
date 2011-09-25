// StorageFacility.cpp
// Implements the StorageFacility class
#include <iostream>

#include "StorageFacility.h"

#include "Logician.h"
#include "GenException.h"
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
  incommod_ = LI->getCommodity(commod_name);
  if (NULL == incommod_)
    throw GenException("Input commodity '" + commod_name 
                       + "' does not exist for facility '" + getName() 
                       + "'.");
  
  inventory_size_ = atof(XMLinput->get_xpath_content(cur,"inventorysize"));
  capacity_ = atof(XMLinput->get_xpath_content(cur,"capacity"));
  residence_time_ = atof(XMLinput->get_xpath_content(cur,"residencetime"));


  inventory_ = deque<Material*>();
  stocks_ = deque<Material*>();
  ordersWaiting_ = deque<Message*>();
  
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
  cout << "stores commodity {"
      << incommod_->getName()
      << "}, for a minimum time of " 
      << residence_time_ 
      << " months and has an inventory_ that holds " 
      << inventory__size_ << " materials."
      << endl;
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void StorageFacility::receiveMessage(Message* msg)
{
  // is this a message from on high? 
  if(msg->getSupplierID()==this->getSN()){
    // file the order
    ordersWaiting_.push_front(msg);
  }
  else {
    throw GenException("StorageFacility is not the supplier of this msg.");
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void StorageFacility::sendMaterial(Message* order, const Communicator* requester)
{
  Transaction trans = order->getTrans();
  // it should be of incommod Commodity type
  if(trans.commod != incommod_){
    throw GenException("StorageFacility can only send incommodity type materials.");
  }
  // pull materials off of the inventory_ stack until you get the trans amount
  Mass complete = 0;

  // start with an empty manifest
  vector<Material*> toSend;

  while(trans.amount > complete && !inventory_.empty() ){
    Material* m = inventory_.front();

    // if the inventory_ obj isn't larger than the remaining need, send it as is.
    if(m->getTotMass() <= (capacity_ - complete)){
      complete += m->getTotMass();
      toSend.push_back(m);
      cout<<"StorageFacility "<< getSN()
        <<"  is sending a mat with mass: "<< m->getTotMass()<< endl;
      inventory_.pop_front();
    }
    else{ 
      // if the inventory_ obj is larger than the remaining need, split it.
      // start with an empty material
      Material* newMat = new Material(CompMap(), 
          m->getUnits(),
          m->getName(), 
          0, atomBased);
      Material* toAbsorb = m->extractMass(capacity_ - complete);
      complete += toAbsorb->getTotMass();
      newMat->absorb(toAbsorb);
      toSend.push_back(newMat);
      cout<<"StorageFacility "<< getSN()
        <<"  is sending a mat with mass: "<< newMat->getTotMass()<< endl;
    };
  };    

  FacilityModel::sendMaterial( order, toSend );
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void StorageFacility::receiveMaterial(Transaction trans, vector<Material*> manifest)
{
  // grab each material object off of the manifest
  // and move it into the stocks.
  // also record its entry time map in entryTimes deque
  for (vector<Material*>::iterator thisMat=manifest.begin();
       thisMat != manifest.end();
       thisMat++)
  {
    cout<<"StorageFacility " << getSN() << " is receiving material with mass "
        << (*thisMat)->getTotMass() << endl;
    stocks_.push_back(*thisMat);
    entryTimes_.push_back(make_pair(TI->getTime(), *thisMat ));
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void StorageFacility::getInitialState(xmlNodePtr cur)
 {
  xmlNodeSetPtr nodes = XMLinput->get_xpath_elements(cur, "initialstocks/entry");
  string fac_name, commod_name, recipe_name;
  FacilityModel* sending_facility;
  Commodity* commodity;
  Material* recipe;
  double amount, age;
  int i, nNodes = nodes->nodeNr;
  std::cout << "**** nNodes = " << nNodes << std::endl;

  // for each fuel pair, there is an in and an out commodity
  for (int i=0;i<nNodes;i++){
    // get xml node
    xmlNodePtr entry_node = nodes->nodeTab[i];

    // assign each item initially in storage
    sending_facility, commodity, recipe = NULL;
    // facility
    fac_name = XMLinput->get_xpath_content(entry_node,"facility");
    sending_facility = dynamic_cast<FacilityModel*>(LI->getModelByName(fac_name, FACILITY));
    if (NULL == facility){
      throw GenException("Facility '" 
			 + fac_name 
			 + "' is not defined in this problem.");
    }
    // commodity
    commod_name = XMLinput->get_xpath_content(entry_node,"incommodity");
    commodity = LI->getCommodity(commod_name);
    if (NULL == commodity){
      throw GenException("Commodity '" 
			 + commod_name
			 + "' is not defined in this problem.");
    }
    // recipe
    recipe_name = XMLinput->get_xpath_content(entry_node,"recipe");
    recipe = LI->getRecipe(recipe_name);
    if (NULL == recipe){
      throw GenException("Recipe '" 
			 + recipe_name
			 + "' is not defined in this problem.");
    }
    // amount
    amount = atof(XMLinput->get_xpath_content(entry_node,"amount"));
    // time in storage (age) in months
    age = atof(XMLinput->get_xpath_content(entry_node,"age"));

    // make new material
    Material* newMat = new Material(recipe->getMassComp(), 
                                    recipe->getUnits(), 
                                    recipe->getName(),
                                    amount, 
                                    massBased);
    
    // decay the material for the alloted time
    newMat->decay(age);

    vector <Material*> manifest;
    manifest.push_back(newMat);

    /* this needs to be fixed */
    // create the book keeping message
    double price = 0.0, minAmt = 0.0;
    Message* storage_history = 
      new Message(commodity, newMat->getAtomComp(), newMat->getTotMass(), 
		  price, minAmt, sending_facility, this);

    // have the facility send its stocks
    sending_facility->sendMaterial(storage_history,manifest);
  }
  
  std::cout << "\n ** Checking initial stocks of size " << stocks_.size() << " **\n" << std::endl;
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
  Mass inv = this->checkInventory_();
  // and how much is already in its stocks
  Mass sto = this->checkStocks(); 
  cout << "stocks currently at: " << sto << " " << inv << endl;
  // subtract inv and sto from inventory_ max size to get total empty space
  Mass space = inventory__size_ - inv - sto;
  // this will be a request for free stuff
  double commod_price = 0;

  if (space == 0){
    // don't request anything
  }
  else if (space < capacity_){
    Communicator* recipient = dynamic_cast<Communicator*>(incommod_->getMarket());
    // if empty space is less than monthly acceptance capacity
    requestAmt = space;
    // recall that requests have a negative amount
    Message* request = new Message(UP_MSG, incommod_, -requestAmt, minAmt, 
                                     commod_price, this, recipient);
      // pass the message up to the inst
      (request->getInst())->receiveMessage(request);
  }
  // otherwise, the upper bound is the monthly acceptance capacity 
  // minus the amount in stocks.
  else if (space >= capacity_){
    Communicator* recipient = dynamic_cast<Communicator*>(incommod_->getMarket());
    // if empty space is more than monthly acceptance capacity
    requestAmt = capacity_ - sto;
    // recall that requests have a negative amount
    Message* request = new Message(UP_MSG, incommod_, -requestAmt, minAmt, commod_price,
                                   this, recipient); 
    // pass the message up to the inst
    (request->getInst())->receiveMessage(request);
  }
  
  // MAKE OFFERS
  // anything in the inventory_ is old enough to leave 
  Mass offer_amt;
  Mass possInv = inv;

  // if the inventory_ isn't full, then offer what you have
  if (possInv < inventory__size_){
    offer_amt = possInv;
  }
  else {
    offer_amt = inventory__size_; 
  }

  // there is no minimum amount a storage facility may send
  double min_amt = 0;

  // decide what market to offer to
  Communicator* recipient = dynamic_cast<Communicator*>(incommod_->getMarket());

  // create a message to go up to the market with these parameters
  Message* msg = new Message(UP_MSG, incommod_, offer_amt, min_amt, commod_price, 
      this, recipient);

  // send it
  sendMessage(msg);
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void StorageFacility::handleTock(int time)
{
  // CHECK MATERIAL AGES
  // Some materials in the stocks have reached their minimum residence time. 
  // put them in the inventory_
  bool someOld = true;
  while( someOld == true && !stocks_.empty()){
    Material* oldEnough = stocks_.front();
    if(TI->getTime() - entryTimes_.front().first >= residence_time_ ){
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
    Message* order = ordersWaiting_.front();
    sendMaterial(order, dynamic_cast<Communicator*>(LI->getModelByID(order->getRequesterID(), FACILITY)));
    ordersWaiting_.pop_front();
  }
  
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
Mass StorageFacility::checkInventory(){
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
Mass StorageFacility::checkStocks(){
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


