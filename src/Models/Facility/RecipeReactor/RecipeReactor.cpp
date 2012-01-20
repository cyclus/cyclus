// RecipeReactor.cpp
// Implements the RecipeReactor class
#include <iostream>
#include "Logger.h"

#include "RecipeReactor.h"

#include "GenericResource.h"
#include "CycException.h"
#include "InputXML.h"
#include "Timer.h"

/*
 * TICK
 * if stocks are empty, ask for a batch
 * offer anything in the inventory
 * if we're at the end of a cycle
 *    - begin the cycle
 *      - move currCore batch to inventory
 *      - move stocks batch to currCore
 *      - reset month_in_cycle clock
 *
 * TOCK
 * advance month_in_cycle
 * send appropriate materials to fill ordersWaiting.
 *
 * RECIEVE MATERIAL
 * put it in stocks
 *
 * SEND MATERIAL
 * pull it from inventory
 *
 * 
 */

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void RecipeReactor::init(xmlNodePtr cur) { 
  FacilityModel::init(cur);
  
  // set the current month in cycle to 1, it's the first month.
  month_in_cycle_ = 1;
  cycle_time_ = 3;

  // move XML pointer to current model
  cur = XMLinput->get_xpath_element(cur,"model/RecipeReactor");

  // initialize ordinary objects
  capacity_ = strtod(XMLinput->get_xpath_content(cur,"capacity"), NULL);
  //cycle_time_ = strtod(XMLinput->get_xpath_content(cur,"cycletime"), NULL);
  lifetime_ = strtol(XMLinput->get_xpath_content(cur,"lifetime"), NULL, 10);
  startConstrYr_ = strtol(XMLinput->get_xpath_content(cur,"startConstrYear"), NULL, 10);
  startConstrMo_ = strtol(XMLinput->get_xpath_content(cur,"startConstrMonth"), NULL, 10);
  startOpYr_ = strtol(XMLinput->get_xpath_content(cur,"startOperYear"), NULL, 10);
  startOpMo_ = strtol(XMLinput->get_xpath_content(cur,"startOperMonth"), NULL, 10);
  licExpYr_ = strtol(XMLinput->get_xpath_content(cur,"licExpYear"), NULL, 10);
  licExpMo_ = strtol(XMLinput->get_xpath_content(cur,"licExpMonth"), NULL, 10);
  state_ = XMLinput->get_xpath_content(cur,"state");
  typeReac_ = XMLinput->get_xpath_content(cur,"typeReac");
  CF_ = strtod(XMLinput->get_xpath_content(cur,"elecCF"), NULL);

  // all facilities require commodities - possibly many
  std::string recipe_name;
  std::string in_commod;
  std::string out_commod;
  xmlNodeSetPtr nodes = XMLinput->get_xpath_elements(cur, "fuelpair");

  // for each fuel pair, there is an in and an out commodity
  for (int i = 0; i < nodes->nodeNr; i++){
    xmlNodePtr pair_node = nodes->nodeTab[i];

    // get commods
    in_commod = XMLinput->get_xpath_content(pair_node,"incommodity");
    out_commod = XMLinput->get_xpath_content(pair_node,"outcommodity");

    // get in_recipe
    recipe_name = XMLinput->get_xpath_content(pair_node,"inrecipe");
    in_recipe_ = IsoVector::recipe(recipe_name);

    // get out_recipe
    recipe_name = XMLinput->get_xpath_content(pair_node,"outrecipe");
    out_recipe_ = IsoVector::recipe(recipe_name);

    fuelPairs_.push_back(make_pair(make_pair(in_commod,in_recipe_),
          make_pair(out_commod, out_recipe_)));
  };

  stocks_ = deque<InFuel>();
  currCore_ = deque<InFuel>();
  inventory_ = deque<OutFuel>();
  ordersWaiting_ = deque<Message*>();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void RecipeReactor::copy(RecipeReactor* src) {

  FacilityModel::copy(src);

  fuelPairs_ = src->fuelPairs_;
  capacity_ = src->capacity_;
  cycle_time_ = src->cycle_time_;
  lifetime_ = src->lifetime_;
  month_in_cycle_ = src->month_in_cycle_;
  startConstrYr_ = src->startConstrYr_;
  startOpYr_ = src->startOpYr_;
  startOpMo_ = src->startOpMo_;
  licExpYr_ = src->licExpYr_;
  licExpMo_ = src->licExpMo_;
  state_ = src->state_;
  typeReac_ = src->typeReac_;
  CF_ = src->CF_;


  stocks_ = deque<InFuel>();
  currCore_ = deque< pair<std::string, Material* > >();
  inventory_ = deque<OutFuel >();
  ordersWaiting_ = deque<Message*>();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void RecipeReactor::copyFreshModel(Model* src) {
  copy(dynamic_cast<RecipeReactor*>(src));
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void RecipeReactor::print() { 
  FacilityModel::print(); 
  LOG(LEV_DEBUG2) << "    converts commodity {"
      << fuelPairs_.front().first.first
      << "} into commodity {"
      << this->fuelPairs_.front().second.first
      << "}.";
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void RecipeReactor::beginCycle() {
  if ( !stocks_.empty() ) {
    // move stocks batch to currCore
    std::string batchCommod = stocks_.front().first;
    Material* batchMat = stocks_.front().second;
    stocks_.pop_front();
    InFuel inBatch;
    inBatch = make_pair(batchCommod, batchMat);
    currCore_.push_back(inBatch);
    // reset month_in_cycle_ clock
    month_in_cycle_ = 1;
  } else{
    // wait for a successful transaction to fill the stocks.
    // reset the cycle month to zero 
    month_in_cycle_=0;
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void RecipeReactor::endCycle() {
  // move a batch out of the core 
  std::string batchCommod = currCore_.front().first;
  Material* batchMat = currCore_.front().second;
  currCore_.pop_front();

  // figure out the spent fuel commodity and material
  std::string outCommod;
  IsoVector outComp;

  for (deque< pair< InRecipe , OutRecipe> >::iterator iter = fuelPairs_.begin();
      iter != fuelPairs_.end();
      iter++) {
    if (iter->first.first == batchCommod) {
      outCommod = iter->second.first;
      outComp = iter->second.second;
      break;
    }
  }

  // change the composition to the compositon of the spent fuel type
  batchMat = new Material(outComp);

  // move converted material into Inventory
  OutFuel outBatch;
  outBatch = make_pair(outCommod, batchMat);
  inventory_.push_back(outBatch);
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void RecipeReactor::receiveMessage(Message* msg) {
  // is this a message from on high? 
  if(msg->supplier()==this){
    // file the order
    ordersWaiting_.push_front(msg);
  }
  else {
    throw CycException("RecipeReactor is not the supplier of this msg.");
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
std::vector<Resource*> RecipeReactor::removeResource(Message* msg) {
  Transaction trans = msg->trans();

  double newAmt = 0;

  Material* m;
  Material* newMat;
  Material* toAbsorb;

  // start with an empty manifest
  vector<Resource*> toSend;

  // pull materials off of the inventory stack until you get the trans amount
  while (trans.resource->quantity() > newAmt && !inventory_.empty() ) {
    for (deque<OutFuel>::iterator iter = inventory_.begin(); 
        iter != inventory_.end(); 
        iter ++){
      // be sure to get the right commodity
      if (iter->first == trans.commod) {
        m = iter->second;

        // start with an empty material
        newMat = new Material();

        // if the inventory obj isn't larger than the remaining need, send it as is.
        if (m->quantity() <= (trans.resource->quantity() - newAmt)) {
          newAmt += m->quantity();
          newMat->absorb(m);
          inventory_.pop_front();
        } else { 
          // if the inventory obj is larger than the remaining need, split it.
          toAbsorb = m->extract(trans.resource->quantity() - newAmt);
          newAmt += toAbsorb->quantity();
          newMat->absorb(toAbsorb);
        }
        toSend.push_back(newMat);
        LOG(LEV_DEBUG2) <<"RecipeReactor "<< ID()
          <<"  is sending a mat with mass: "<< newMat->quantity();
      }
    }
  }    
  return toSend;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void RecipeReactor::addResource(Message* msg, vector<Resource*> manifest) {
  // grab each material object off of the manifest
  // and move it into the stocks.
  for (vector<Resource*>::iterator thisMat=manifest.begin();
       thisMat != manifest.end();
       thisMat++) {
    LOG(LEV_DEBUG2) <<"RecipeReactor " << ID() << " is receiving material with mass "
        << (*thisMat)->quantity();
    stocks_.push_front(make_pair(msg->trans().commod, dynamic_cast<Material*>(*thisMat)));
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void RecipeReactor::handleTick(int time) {
  // if at beginning of cycle, beginCycle()
  // if stocks are empty, ask for a batch
  // offer anything in the inventory
  
  // BEGIN CYCLE
  if(month_in_cycle_ == 1){
    this->beginCycle();
  };

  makeRequests();
  makeOffers();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void RecipeReactor::makeRequests(){
  // MAKE A REQUEST
  if(this->checkStocks() == 0){
    // It chooses the next in/out commodity pair in the preference lineup
    InRecipe request_commod_pair;
    OutRecipe offer_commod_pair;
    request_commod_pair = fuelPairs_.front().first;
    offer_commod_pair = fuelPairs_.front().second;
    std::string in_commod = request_commod_pair.first;
    IsoVector in_recipe = request_commod_pair.second;

    // It then moves that pair from the front to the back of the preference lineup
    fuelPairs_.push_back(make_pair(request_commod_pair, offer_commod_pair));
    fuelPairs_.pop_front();

    // It can accept only a whole batch
    double requestAmt;
    double minAmt = in_recipe.mass();
    // The Recipe Reactor should ask for a batch if there isn't one in stock.
    double sto = this->checkStocks(); 
    // subtract sto from batch size to get total empty space. 
    // Hopefully the result is either 0 or the batch size 
    double space = minAmt - sto; // KDHFLAG get minAmt from the input ?
    // this will be a request for free stuff
    double commod_price = 0;

    if (space == 0) {
      // don't request anything
    } else if (space <= minAmt) {
      MarketModel* market = MarketModel::marketForCommod(in_commod);
      Communicator* recipient = dynamic_cast<Communicator*>(market);
      // if empty space is less than monthly acceptance capacity
      requestAmt = space;

      // request a generic resource
      GenericResource* request_res = new GenericResource(in_commod, "kg", requestAmt);

      // build the transaction and message
      Transaction trans;
      trans.commod = in_commod;
      trans.minfrac = minAmt/requestAmt;
      trans.is_offer = false;
      trans.price = commod_price;
      trans.resource = request_res;

      sendMessage(recipient, trans);
      // otherwise, the upper bound is the batch size
      // minus the amount in stocks.
    } else if (space >= minAmt) {
      MarketModel* market = MarketModel::marketForCommod(in_commod);
      Communicator* recipient = dynamic_cast<Communicator*>(market);
      // if empty space is more than monthly acceptance capacity
      requestAmt = capacity_ - sto;

      // request a generic resource
      GenericResource* request_res = new GenericResource(in_commod, "kg", requestAmt);

      // build the transaction and message
      Transaction trans;
      trans.commod = in_commod;
      trans.minfrac = minAmt/requestAmt;
      trans.is_offer = false;
      trans.price = commod_price;
      trans.resource = request_res;
      sendMessage(recipient, trans);
    }
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void RecipeReactor::sendMessage(Communicator* recipient, Transaction trans){
      Message* msg = new Message(this, recipient, trans); 
      msg->setNextDest(facInst());
      msg->sendOn();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void RecipeReactor::makeOffers(){
  // MAKE OFFERS
  // decide how much to offer

  // there is no minimum amount a null facility may send
  double min_amt = 0;
  // this will be an offer of free stuff
  double commod_price = 0;

  // there are potentially many types of batch in the inventory stack
  double inv = this->checkInventory();
  // send an offer for each material on the stack 
  std::string commod;
  Communicator* recipient;
  double offer_amt;
  for (deque<pair<std::string, Material* > >::iterator iter = inventory_.begin(); 
       iter != inventory_.end(); 
       iter ++){
    // get commod
    commod = iter->first;
    MarketModel* market = MarketModel::marketForCommod(commod);
    // decide what market to offer to
    recipient = dynamic_cast<Communicator*>(market);
    // get amt
    offer_amt = iter->second->quantity();

    // make a material to offer
    Material* offer_mat = new Material(out_recipe_);
    offer_mat->setQuantity(offer_amt);

    // build the transaction and message
    Transaction trans;
    trans.commod = commod;
    trans.minfrac = min_amt/offer_amt;
    trans.is_offer = true;
    trans.price = commod_price;
    trans.resource = offer_mat;

    sendMessage(recipient, trans);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void RecipeReactor::handleTock(int time) {
  // at the end of the cycle
  if (month_in_cycle_ > cycle_time_){
    this->endCycle();
  };

  // check what orders are waiting, 
  while(!ordersWaiting_.empty()){
    Message* order = ordersWaiting_.front();
    order->approveTransfer();
    ordersWaiting_.pop_front();
  };
  month_in_cycle_++;

  // call the facility model's handle tock last 
  // to check for decommissioning
  FacilityModel::handleTock(time);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
double RecipeReactor::checkInventory(){
  double total = 0;

  // Iterate through the inventory and sum the amount of whatever
  // material unit is in each object.

  for (deque< pair<std::string, Material*> >::iterator iter = inventory_.begin(); 
       iter != inventory_.end(); 
       iter ++){
    total += iter->second->quantity();
  }

  return total;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
double RecipeReactor::checkStocks(){
  double total = 0;

  // Iterate through the stocks and sum the amount of whatever
  // material unit is in each object.

  if(!stocks_.empty()){
    for (deque< pair<std::string, Material*> >::iterator iter = stocks_.begin(); 
         iter != stocks_.end(); 
         iter ++){
        total += iter->second->quantity();
    };
  };
  return total;
}


/* --------------------
   output database info
 * --------------------
 */
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
std::string RecipeReactor::outputDir_ = "/recipeReactor";


/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

extern "C" Model* constructRecipeReactor() {
  return new RecipeReactor();
}

extern "C" void destructRecipeReactor(Model* p) {
  delete p;
}

/* ------------------- */ 

