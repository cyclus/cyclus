// RecipeReactor.cpp
// Implements the RecipeReactor class
#include <iostream>

#include "RecipeReactor.h"

#include "Logger.h"
#include "RecipeLogger.h"
#include "GenericResource.h"
#include "CycException.h"
#include "InputXML.h"
#include "Timer.h"

using namespace std;

/**
  TICK
  if stocks are empty, ask for a batch
  offer anything in the inventory
  if we're at the end of a cycle
     - begin the cycle
       - move currCore batch to inventory
       - move stocks batch to currCore
       - reset month_in_cycle clock
 
  TOCK
  advance month_in_cycle
  send appropriate materials to fill ordersWaiting.
 
  RECIEVE MATERIAL
  put it in stocks
 
  SEND MATERIAL
  pull it from inventory
  
 */

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
RecipeReactor::RecipeReactor() {
  // set the current month in cycle to 1, it's the first month.
  month_in_cycle_ = 1;
  cycle_length_ = 3;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void RecipeReactor::init(xmlNodePtr cur) { 
  FacilityModel::init(cur);
  
  // move XML pointer to current model
  cur = XMLinput->get_xpath_element(cur,"model/RecipeReactor");

  // initialize ordinary objects
  capacity_ = strtod(XMLinput->get_xpath_content(cur,"capacity"), NULL);
  //cycle_length_ = strtod(XMLinput->get_xpath_content(cur,"cycletime"), NULL);
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
  string recipe_name;
  string in_commod;
  string out_commod;
  xmlNodeSetPtr nodes = XMLinput->get_xpath_elements(cur, "fuelpair");

  // for each fuel pair, there is an in and an out commodity
  for (int i = 0; i < nodes->nodeNr; i++){
    xmlNodePtr pair_node = nodes->nodeTab[i];

    // get commods
    in_commod = XMLinput->get_xpath_content(pair_node,"incommodity");
    out_commod = XMLinput->get_xpath_content(pair_node,"outcommodity");

    // get in_recipe
    recipe_name = XMLinput->get_xpath_content(pair_node,"inrecipe");
    in_recipe_ = RecipeLogger::Recipe(recipe_name);

    // get out_recipe
    recipe_name = XMLinput->get_xpath_content(pair_node,"outrecipe");
    out_recipe_ = RecipeLogger::Recipe(recipe_name);

    fuelPairs_.push_back(make_pair(make_pair(in_commod,in_recipe_),
          make_pair(out_commod, out_recipe_)));
  };

  stocks_ = deque<Fuel>();
  currCore_ = deque<Fuel>();
  inventory_ = deque<Fuel>();
  ordersWaiting_ = deque<msg_ptr>();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void RecipeReactor::copy(RecipeReactor* src) {

  FacilityModel::copy(src);

  fuelPairs_ = src->fuelPairs_;
  capacity_ = src->capacity_;
  cycle_length_ = src->cycle_length_;
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


  stocks_ = deque<Fuel>();
  currCore_ = deque< pair<string, mat_rsrc_ptr > >();
  inventory_ = deque<Fuel >();
  ordersWaiting_ = deque<msg_ptr>();

  in_recipe_ = src->in_recipe_;
  out_recipe_ = src->out_recipe_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void RecipeReactor::copyFreshModel(Model* src) {
  copy(dynamic_cast<RecipeReactor*>(src));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
std::string RecipeReactor::str() { 
  std::string s = FacilityModel::str(); 
  s += "    converts commodity '"
    + fuelPairs_.front().first.first
    + "' into commodity '"
    + this->fuelPairs_.front().second.first
    + "'.";
  return s;
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void RecipeReactor::beginCycle() {
  if ( !stocks_.empty() ) {
    // move stocks batch to currCore
    string batchCommod = stocks_.front().first;
    mat_rsrc_ptr batchMat = stocks_.front().second;
    stocks_.pop_front();
    Fuel inBatch;
    inBatch = make_pair(batchCommod, batchMat);
    LOG(LEV_DEBUG2, "RReact") << "Adding a new batch to the core";
    currCore_.push_back(inBatch);
    // reset month_in_cycle_ clock
    month_in_cycle_ = 1;
  } else {
    LOG(LEV_DEBUG3, "RReact") << "Beginning a cycle with an empty core. Why??";
    // wait for a successful transaction to fill the stocks.
    // reset the cycle month to zero 
    month_in_cycle_=0;
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void RecipeReactor::endCycle() {
  LOG(LEV_DEBUG2, "RReact") << "Ending a cycle.";
  month_in_cycle_ = 0;
  if (currCore_.size() == 0) {
    LOG(LEV_DEBUG3, "RReact") << "Ended a cycle with an empty core. Why??";
    return;
  }

  // move a batch out of the core 
  string batchCommod = currCore_.front().first;
  mat_rsrc_ptr batchMat = currCore_.front().second;
  currCore_.pop_front();

  // figure out the spent fuel commodity and material
  string outCommod;
  IsoVector outComp;

  for (deque< pair< Recipe , Recipe> >::iterator iter = fuelPairs_.begin();
      iter != fuelPairs_.end();
      iter++) {
    if (iter->first.first == batchCommod) {
      outCommod = iter->second.first;
      outComp = iter->second.second;
      break;
    }
  }

  // change the composition to the compositon of the spent fuel type
  batchMat = mat_rsrc_ptr(new Material(outComp));

  // move converted material into Inventory
  Fuel outBatch;
  outBatch = make_pair(outCommod, batchMat);
  inventory_.push_back(outBatch);
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void RecipeReactor::receiveMessage(msg_ptr msg) {
  // is this a message from on high? 
  if(msg->trans().supplier()==this){
    // file the order
    ordersWaiting_.push_front(msg);
    LOG(LEV_INFO5, "RReact") << name() << " just received an order.";
  }
  else {
    throw CycException("RecipeReactor is not the supplier of this msg.");
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
vector<rsrc_ptr> RecipeReactor::removeResource(msg_ptr msg) {
  Transaction trans = msg->trans();

  double newAmt = 0;

  mat_rsrc_ptr m;
  mat_rsrc_ptr newMat;
  mat_rsrc_ptr toAbsorb;

  // start with an empty manifest
  vector<rsrc_ptr> toSend;

  // pull materials off of the inventory stack until you get the trans amount
  while (trans.resource()->quantity() > newAmt && !inventory_.empty() ) {
    for (deque<Fuel>::iterator iter = inventory_.begin(); 
        iter != inventory_.end(); 
        iter ++){
      // be sure to get the right commodity
      if (iter->first == trans.commod()) {
        m = iter->second;

        // start with an empty material
        newMat = mat_rsrc_ptr(new Material());

        // if the inventory obj isn't larger than the remaining need, send it as is.
        if (m->quantity() <= (trans.resource()->quantity() - newAmt)) {
          newAmt += m->quantity();
          newMat->absorb(m);
          inventory_.pop_front();
        } else { 
          // if the inventory obj is larger than the remaining need, split it.
          toAbsorb = m->extract(trans.resource()->quantity() - newAmt);
          newAmt += toAbsorb->quantity();
          newMat->absorb(toAbsorb);
        }
        toSend.push_back(newMat);
        LOG(LEV_DEBUG2, "RReact") <<"RecipeReactor "<< ID()
          <<"  is sending a mat with mass: "<< newMat->quantity();
      }
    }
  }    
  return toSend;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void RecipeReactor::addResource(msg_ptr msg, std::vector<rsrc_ptr> manifest) {
  // grab each material object off of the manifest
  // and move it into the stocks.
  for (vector<rsrc_ptr>::iterator thisMat=manifest.begin();
       thisMat != manifest.end();
       thisMat++) {
    LOG(LEV_DEBUG2, "RReact") <<"RecipeReactor " << ID() << " is receiving material with mass "
        << (*thisMat)->quantity();
    stocks_.push_front(make_pair(msg->trans().commod(), boost::dynamic_pointer_cast<Material>(*thisMat)));
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void RecipeReactor::handleTick(int time) {
  LOG(LEV_INFO3, "RReact") << name() << " is ticking {";

  // if at beginning of cycle, beginCycle()
  // if stocks are empty, ask for a batch
  // offer anything in the inventory
  
  // BEGIN CYCLE
  if(month_in_cycle_ == 1){
    LOG(LEV_INFO4, "RReact") << " Beginning a new cycle";
    this->beginCycle();
  };

  makeRequests();
  makeOffers();
  LOG(LEV_INFO3, "RReact") << "}";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void RecipeReactor::makeRequests(){
  // MAKE A REQUEST
  if(this->stocksMass() != 0) {
    return;
  }

  // It chooses the next in/out commodity pair in the preference lineup
  Recipe request_commod_pair;
  Recipe offer_commod_pair;
  request_commod_pair = fuelPairs_.front().first;
  offer_commod_pair = fuelPairs_.front().second;
  string in_commod = request_commod_pair.first;
  IsoVector in_recipe = request_commod_pair.second;

  // It then moves that pair from the front to the back of the preference lineup
  fuelPairs_.push_back(make_pair(request_commod_pair, offer_commod_pair));
  fuelPairs_.pop_front();

  // It can accept only a whole batch
  double requestAmt;
  double minAmt = in_recipe.mass();
  // The Recipe Reactor should ask for a batch if there isn't one in stock.
  double sto = this->stocksMass(); 
  // subtract sto from batch size to get total empty space. 
  // Hopefully the result is either 0 or the batch size 
  double space = minAmt - sto; // KDHFLAG get minAmt from the input ?
  // this will be a request for free stuff
  double commod_price = 0;

  if (space <= 0) {
    // don't request anything
    return;
  } else if (space <= minAmt) {
    // if empty space is less than monthly acceptance capacity
    requestAmt = space;
  } else if (space >= minAmt) {
    // empty space is more than monthly acceptance capacity
    // upper bound is the batch size minus the amount in stocks.
    requestAmt = capacity_ - sto;
  }

  LOG(LEV_INFO4, "RReact") << " making requests {";

  MarketModel* market = MarketModel::marketForCommod(in_commod);
  Communicator* recipient = dynamic_cast<Communicator*>(market);

  // request a generic resource
  gen_rsrc_ptr request_res = gen_rsrc_ptr(new GenericResource(in_commod, "kg", requestAmt));

  // build the transaction and message
  Transaction trans(this, REQUEST);
  trans.setCommod(in_commod);
  trans.minfrac = minAmt/requestAmt;
  trans.setPrice(commod_price);
  trans.setResource(request_res);

  LOG(LEV_INFO5, "RReact") << name() << " has requested " << request_res->quantity()
                           << " kg of " << in_commod << ".";
  sendMessage(recipient, trans);
  LOG(LEV_INFO4, "RReact") << "}";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void RecipeReactor::sendMessage(Communicator* recipient, Transaction trans){
      msg_ptr msg(new Message(this, recipient, trans)); 
      msg->setNextDest(facInst());
      msg->sendOn();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void RecipeReactor::makeOffers(){
  LOG(LEV_INFO4, "RReact") << " making offers {";
  // MAKE OFFERS
  // decide how much to offer

  // there is no minimum amount a null facility may send
  double min_amt = 0;
  // this will be an offer of free stuff
  double commod_price = 0;

  // there are potentially many types of batch in the inventory stack
  double inv = this->inventoryMass();
  // send an offer for each material on the stack 
  string commod;
  Communicator* recipient;
  double offer_amt;
  for (deque<pair<string, mat_rsrc_ptr > >::iterator iter = inventory_.begin(); 
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
    mat_rsrc_ptr offer_mat = mat_rsrc_ptr(new Material(out_recipe_));
    offer_mat->print();
    offer_mat->setQuantity(offer_amt);

    // build the transaction and message
    Transaction trans(this, OFFER);
    trans.setCommod(commod);
    trans.minfrac = min_amt/offer_amt;
    trans.setPrice(commod_price);
    trans.setResource(offer_mat);

    LOG(LEV_INFO5, "RReact") << name() << " has offered " << offer_mat->quantity()
                             << " kg of " << commod << ".";

    sendMessage(recipient, trans);
  }
  LOG(LEV_INFO4, "RReact") << "}";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void RecipeReactor::handleTock(int time) {
  LOG(LEV_INFO3, "RReact") << name() << " is tocking {";
  // at the end of the cycle
  if (month_in_cycle_ > cycle_length_){
    this->endCycle();
  };

  // check what orders are waiting, 
  while(!ordersWaiting_.empty()){
    msg_ptr order = ordersWaiting_.front();
    order->approveTransfer();
    ordersWaiting_.pop_front();
  };
  month_in_cycle_++;
  LOG(LEV_INFO3, "RReact") << "}";
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    

int RecipeReactor::cycleLength() {
  return cycle_length_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void RecipeReactor::setCycleLength(int length) {
  cycle_length_ = length;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
double RecipeReactor::capacity() {
  return capacity_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void RecipeReactor::setCapacity(double cap) {
  capacity_ = cap;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
double RecipeReactor::inventorySize() {
  return inventory_size_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void RecipeReactor::setInventorySize(double size) {
  inventory_size_ = size;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
int RecipeReactor::facLife() {
  return lifetime_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void RecipeReactor::setFacLife(int lifespan) {
  lifetime_ = lifespan;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
double RecipeReactor::capacityFactor() {
  return CF_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void RecipeReactor::setCapacityFactor(double cf) {
  CF_ = cf;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
IsoVector RecipeReactor::inRecipe() {
  return in_recipe_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void RecipeReactor::setInRecipe(IsoVector recipe) {
  in_recipe_ = recipe;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
IsoVector RecipeReactor::outRecipe() {
  return out_recipe_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void RecipeReactor::setOutRecipe(IsoVector recipe) {
  out_recipe_ = recipe;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void RecipeReactor::addFuelPair(std::string incommod, IsoVector inFuel,
                                std::string outcommod, IsoVector outFuel) {
  fuelPairs_.push_back(make_pair(make_pair(incommod, inFuel),
                                 make_pair(outcommod, outFuel)));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
string RecipeReactor::inCommod() {
  return fuelPairs_.front().first.first ;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
string RecipeReactor::outCommod() {
  return fuelPairs_.front().second.first;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
double RecipeReactor::inventoryMass(){
  double total = 0;

  // Iterate through the inventory and sum the amount of whatever
  // material unit is in each object.

  for (deque< pair<string, mat_rsrc_ptr> >::iterator iter = inventory_.begin(); 
       iter != inventory_.end(); 
       iter ++){
    total += iter->second->quantity();
  }

  return total;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
double RecipeReactor::stocksMass(){
  double total = 0;

  // Iterate through the stocks and sum the amount of whatever
  // material unit is in each object.

  if(!stocks_.empty()){
    for (deque< pair<string, mat_rsrc_ptr> >::iterator iter = stocks_.begin(); 
         iter != stocks_.end(); 
         iter ++){
        total += iter->second->quantity();
    };
  };
  return total;
}

/* ------------------- */ 


/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

extern "C" Model* constructRecipeReactor() {
  return new RecipeReactor();
}

/* ------------------- */ 

