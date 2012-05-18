// BatchReactor.cpp
// Implements the BatchReactor class
#include "BatchReactor.h"

#include "Logger.h"
#include "GenericResource.h"
#include "RecipeLogger.h"
#include "CycException.h"
#include "InputXML.h"
#include "Timer.h"

#include <queue>
#include <sstream>

using namespace std;

/**
  TICK
  TOCK
  RECIEVE MATERIAL
  SEND MATERIAL
 */

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
BatchReactor::BatchReactor() {
  init();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void BatchReactor::init() { 
  pre_core_.makeUnlimited();
  in_core_.makeUnlimited();
  wet_storage_.makeUnlimited();
  dry_storage_.makeUnlimited();
  post_core_.makeUnlimited();
  request_amount_ = 0.0;
  lifetime_ = 0;
  operation_timer_ = -1;
  phase_ = INIT;
  transfers_ = queue<FuelTransfer>();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void BatchReactor::init(xmlNodePtr cur) { 
  FacilityModel::init(cur);
  
  // move XML pointer to current model
  cur = XMLinput->get_xpath_element(cur,"model/BatchReactor");
  
  // initialize facility parameters
  setCycleLength( strtod( XMLinput->get_xpath_content(cur,"cyclelength"), NULL ) );
  setLifetime( strtol( XMLinput->get_xpath_content(cur,"lifetime"), NULL, 10 ) ); 
  setCoreLoading( strtod( XMLinput->get_xpath_content(cur,"coreloading"), NULL ) );
  setNBatches(strtol( XMLinput->get_xpath_content(cur,"batchespercore"), NULL, 10 ) ); 
  setBatchLoading( core_loading_ / batches_per_core_ );
  setWetResidency( strtol( XMLinput->get_xpath_content(cur,"wetresidency"), NULL, 10 ) );
  setDryResidency( strtol( XMLinput->get_xpath_content(cur,"dryresidency"), NULL, 10 ) );
  setOperationTimer(0);

  // all facilities require commodities - possibly many
  string recipe_name;
  string in_commod;
  string out_commod;
  xmlNodeSetPtr nodes = XMLinput->get_xpath_elements(cur, "fuelpair");

  // for each fuel pair, there is an in and an out commodity
  for (int i = 0; i < nodes->nodeNr; i++) {
    xmlNodePtr pair_node = nodes->nodeTab[i];

    // get commods
    in_commod = XMLinput->get_xpath_content(pair_node,"incommodity");
    out_commod = XMLinput->get_xpath_content(pair_node,"outcommodity");

    // get in_recipe
    recipe_name = XMLinput->get_xpath_content(pair_node,"inrecipe");
    setInRecipe(RecipeLogger::Recipe(recipe_name));

    // get out_recipe
    recipe_name = XMLinput->get_xpath_content(pair_node,"outrecipe");
    setOutRecipe(RecipeLogger::Recipe(recipe_name));

    fuel_pairs_.push_back(make_pair(make_pair(in_commod,in_recipe_),
          make_pair(out_commod, out_recipe_)));
  }

  setPhase(BEGIN);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void BatchReactor::copy(BatchReactor* src) {

  FacilityModel::copy(src);

  setCycleLength( src->cycleLength() ); 
  setLifetime( src->lifetime() );
  setCoreLoading( src->coreLoading() );
  setNBatches( src->nBatches() );
  setBatchLoading( coreLoading() / nBatches() ); 
  setInRecipe( src->inRecipe() );
  setOutRecipe( src->outRecipe() );
  wet_residency_ = src->wet_residency_;
  dry_residency_ = src->dry_residency_;
  setOperationTimer(0);
  fuel_pairs_ = src->fuel_pairs_;

  setPhase(BEGIN);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void BatchReactor::copyFreshModel(Model* src) {
  copy(dynamic_cast<BatchReactor*>(src));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
std::string BatchReactor::str() { 
  std::stringstream ss;
  ss << FacilityModel::str();
  ss << " has facility parmeters {"
     << "Lifetime = " << lifetime()
     << ", Cycle Length = " << cycleLength()
     << ", Core Loading = " << coreLoading()
     << ", Batches Per Core = " << nBatches()
     << ", Batch Loading = " << batchLoading()
     << ", converts commodity '";
  if (fuel_pairs_.size() > 0) {
    ss << fuel_pairs_.front().first.first
       << "' into commodity '"
       << this->fuel_pairs_.front().second.first;
  }
  ss << "'}";
  return ss.str();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void BatchReactor::receiveMessage(msg_ptr msg) {
  // is this a message from on high? 
  if(msg->trans().supplier()==this){
    // file the order
    orders_waiting_.push_front(msg);
    LOG(LEV_INFO5, "BReact") << name() << " just received an order.";
  }
  else {
    throw CycException("BatchReactor is not the supplier of this msg.");
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void BatchReactor::sendMessage(Communicator* recipient, Transaction trans){
      msg_ptr msg(new Message(this, recipient, trans)); 
      msg->setNextDest(facInst());
      msg->sendOn();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void BatchReactor::handleOrders() {
  while(!orders_waiting_.empty()){
    msg_ptr order = orders_waiting_.front();
    order->approveTransfer();
    orders_waiting_.pop_front();
  };
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void BatchReactor::addResource(msg_ptr msg,
                               std::vector<rsrc_ptr> manifest) {
  double preQuantity = pre_core_.quantity();
  pre_core_.pushAll(MatBuff::toMat(manifest));
  double added = pre_core_.quantity() - preQuantity;
  LOG(LEV_DEBUG4, "BReact") << "BatchReactor " << name() << " added "
                            << added << " to its precore buffer.";
}
  
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
vector<rsrc_ptr> BatchReactor::removeResource(msg_ptr order) {
  Transaction trans = order->trans();
  double amt = trans.resource()->quantity();

  LOG(LEV_DEBUG4, "BReact") << "BatchReactor " << name() << " removed "
                            << amt << " of " << post_core_.quantity() 
                            << " to its postcore buffer.";
  
  return MatBuff::toRes(post_core_.popQty(amt));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void BatchReactor::handleTick(int time) {
  LOG(LEV_INFO3, "BReact") << name() << " is ticking at time " << time << " {";

  // transfer fuel if we need to
  while (transfers_.front().time == TI->time()) {
    FuelTransfer t = transfers_.front();
    switch(t.end_location) {
    case(DRY):
      moveFuel(wet_storage_,dry_storage_,t.quantity,t.end_location);
      break;
    case(OUT):
      moveFuel(dry_storage_,post_core_,t.quantity,t.end_location);
      break;
    transfers_.pop();
    }
  }

  // end the facility's life if its time
  if (lifetimeReached()) {
    setPhase(END);
  }
  // request fuel if needed
  if (requestAmt() > EPS_KG) {
    makeRequest(requestAmt());
  }
  // offer used fuel if needed
  if (!post_core_.empty()) {
    makeOffers();
  }
  
  LOG(LEV_INFO3, "BReact") << "}";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void BatchReactor::handleTock(int time) { 
  LOG(LEV_INFO3, "BReact") << name() << " is tocking {";
  LOG(LEV_DEBUG3, "BReact") << "The current phase is: " << phase();
  
  handleOrders();

  switch(phase()) {
    case BEGIN:
    case REFUEL:
      if (requestMet()) {
        loadCore();
        setPhase(OPERATION);
      }
      else { 
        setRequestAmt(requestAmt() - receivedAmt());
      } 
      break; // end BEGIN || REFUEL 
    case OPERATION:
      increaseCycleTimer();
      if (cycleComplete()) {
        setPhase(REFUEL);
      }
      break; // end OPERATION
    case END:
      if (post_core_.empty()) {
        decomission();
        return;
      }
      break; // end END
  }
  increaseOperationTimer();

  LOG(LEV_INFO3, "BReact") << "}";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void BatchReactor::setPhase(Phase p) {
  LOG(LEV_DEBUG3, "BReact") << "Setting phase to: " << p;
  switch (p) {
  case BEGIN:
    setRequestAmt(coreLoading());
    break;
  case REFUEL:
    offloadBatch();
    setRequestAmt(batchLoading());
    break;
  case OPERATION:
    resetRequestAmt();
    resetCycleTimer();
    break;
  case END:
    resetRequestAmt();
    offloadCore();
    break;
  default:
    stringstream err("");
    err << "BatchReactor " << this->name() << " does not have a phase "
        << "enumerated by " << p << ".";
    throw CycOverrideException(err.str()); 
    break;
  }
  phase_ = p;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void BatchReactor::decomission() {
  delete this;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
bool BatchReactor::requestMet() {
  double remaining = requestAmt() - receivedAmt();

  LOG(LEV_DEBUG5, "BReact") << "BatchReactor is determining if a request is "
                            << "met the request amount, received amout, and "
                            << "their difference being ";
  LOG(LEV_DEBUG5, "BReact") << "  * " << requestAmt();
  LOG(LEV_DEBUG5, "BReact") << "  * " << receivedAmt();
  LOG(LEV_DEBUG5, "BReact") << "  * " << remaining;

  if (remaining > EPS_KG) {
    return false;
  }
  else if (remaining < -1*EPS_KG) {
    stringstream err("");
    err << "BatchReactor " << this->name() << " received more fuel than was "
        << "expected, which is not currently acceptable behavior; it has a "
        << "surplus of " << -1*remaining << ".";
    throw CycOverrideException(err.str()); 
  }
  return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void BatchReactor::moveFuel(MatBuff& fromBuff, MatBuff& toBuff, 
                            double amt, Location end_location) {
  vector<mat_rsrc_ptr> popped = fromBuff.popQty(amt);
  switch (end_location) {
  case WET: // transmute, log, push
    {
      mat_rsrc_ptr new_mat = mat_rsrc_ptr(new Material(out_recipe_));
      new_mat->setQuantity(amt);
      addToTable(new_mat,TI->time(),WET);
      toBuff.pushOne(new_mat);
      FuelTransfer trans(TI->time()+dry_residency_,amt,DRY);
      transfers_.push(trans);
      break;
    }
 case DRY: // log, push
    for (vector<mat_rsrc_ptr>::iterator mat = popped.begin();
         mat != popped.end(); mat++) {
      addToTable(*mat,TI->time(),DRY);
    }
 case IN:  // push
 case OUT: // push
    toBuff.pushAll(popped);
    break;  
 default:
    stringstream err("");
    err << "BatchReactor " << this->name() << " does not have a location "
        << "enumerated by " << end_location << ".";
    throw CycOverrideException(err.str()); 
    break;
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void BatchReactor::loadCore() {
  moveFuel(pre_core_,in_core_,pre_core_.quantity(),IN);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void BatchReactor::offloadBatch() {
  FuelTransfer t(TI->time()+wet_residency_,batchLoading(),DRY);
  moveFuel(in_core_,wet_storage_,batchLoading(),WET);
  transfers_.push(t);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void BatchReactor::offloadCore() {
  FuelTransfer t(TI->time()+wet_residency_,in_core_.quantity(),DRY);
  moveFuel(in_core_,wet_storage_,in_core_.quantity(),WET);
  transfers_.push(t);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void BatchReactor::interactWithMarket(std::string commod, double amt, TransType type) {
  LOG(LEV_INFO4, "BReact") << " making requests {";  
  // get the market
  MarketModel* market = MarketModel::marketForCommod(commod);
  Communicator* recipient = dynamic_cast<Communicator*>(market);
  // set the price
  double commod_price = 0;
  // request a generic resource
  gen_rsrc_ptr trade_res = gen_rsrc_ptr(new GenericResource(commod, "kg", amt));
  // build the transaction and message
  Transaction trans(this, type);
  trans.setCommod(commod);
  trans.minfrac = 1.0;
  trans.setPrice(commod_price);
  trans.setResource(trade_res);
  // log the event
  string text;
  if (type == OFFER) {
    text = " has offered ";
  }
  else {
    text = " has requested ";
  }
  LOG(LEV_INFO5, "BReact") << name() << text << amt
                           << " kg of " << commod << ".";
  // send the message
  sendMessage(recipient, trans);
  LOG(LEV_INFO4, "BReact") << "}";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void BatchReactor::addFuelPair(std::string incommod, IsoVector inFuel,
                                std::string outcommod, IsoVector outFuel) {
  fuel_pairs_.push_back(make_pair(make_pair(incommod, inFuel),
                                 make_pair(outcommod, outFuel)));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void BatchReactor::defineTable() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void BatchReactor::addToTable(mat_rsrc_ptr mat, int time, Location l) {
}

/* ------------------- */ 


/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

extern "C" Model* constructBatchReactor() {
  return new BatchReactor();
}

/* ------------------- */ 

