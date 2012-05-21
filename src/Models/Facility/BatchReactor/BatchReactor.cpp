// BatchReactor.cpp
// Implements the BatchReactor class
#include "BatchReactor.h"

#include "Logger.h"
#include "GenericResource.h"
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
  preCore_.makeUnlimited();
  inCore_.makeUnlimited();
  postCore_.makeUnlimited();
  request_amount_ = 0.0;
  lifetime_ = 0;
  operation_timer_ = -1;
  phase_ = INIT;
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
  setNBatches( strtol( XMLinput->get_xpath_content(cur,"batchespercore"), NULL, 10 ) ); 
  setBatchLoading( core_loading_ / batches_per_core_ );
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
    setInRecipe(IsoVector::recipe(recipe_name));

    // get out_recipe
    recipe_name = XMLinput->get_xpath_content(pair_node,"outrecipe");
    setOutRecipe(IsoVector::recipe(recipe_name));

    fuelPairs_.push_back(make_pair(make_pair(in_commod,in_recipe_),
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
  setOperationTimer(0);
  fuelPairs_ = src->fuelPairs_;

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
  if (fuelPairs_.size() > 0) {
    ss << fuelPairs_.front().first.first
       << "' into commodity '"
       << this->fuelPairs_.front().second.first;
  }
  ss << "'}";
  return ss.str();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void BatchReactor::receiveMessage(msg_ptr msg) {
  // is this a message from on high? 
  if(msg->trans().supplier()==this){
    // file the order
    ordersWaiting_.push_front(msg);
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
  while(!ordersWaiting_.empty()){
    msg_ptr order = ordersWaiting_.front();
    order->trans().approveTransfer();
    ordersWaiting_.pop_front();
  };
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void BatchReactor::addResource(Transaction trans,
                               std::vector<rsrc_ptr> manifest) {
  double preQuantity = preCore_.quantity();
  preCore_.pushAll(ResourceBuff::toMat(manifest));
  double added = preCore_.quantity() - preQuantity;
  LOG(LEV_DEBUG4, "BReact") << "BatchReactor " << name() << " added "
                            << added << " to its precore buffer.";
}
  
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
vector<rsrc_ptr> BatchReactor::removeResource(Transaction order) {
  Transaction trans = order;
  double amt = trans.resource()->quantity();

  LOG(LEV_DEBUG4, "BReact") << "BatchReactor " << name() << " removed "
                            << amt << " of " << postCore_.quantity() 
                            << " to its postcore buffer.";
  
  return ResourceBuff::toRes(postCore_.popQty(amt));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void BatchReactor::handleTick(int time) {
  LOG(LEV_INFO3, "BReact") << name() << " is ticking at time " << time << " {";

  // end the facility's life if its time
  if (lifetimeReached()) {
    setPhase(END);
  }
  // request fuel if needed
  if (requestAmt() > EPS_KG) {
    makeRequest(requestAmt());
  }
  // offer used fuel if needed
  if (!postCore_.empty()) {
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
      if (postCore_.empty()) {
        delete this;
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
void BatchReactor::moveFuel(MatBuff& fromBuff, MatBuff& toBuff, double amt) {
  //  toBuff->pushAll(fromBuff->popQty(amt));
  vector<mat_rsrc_ptr> to_delete = fromBuff.popQty(amt);
  IsoVector* temp = &out_recipe_;
  temp->setMass(amt);
  mat_rsrc_ptr newMat = mat_rsrc_ptr(new Material((*temp)));
  //newMat->setMass(amt);
  toBuff.pushOne(newMat);
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
  fuelPairs_.push_back(make_pair(make_pair(incommod, inFuel),
                                 make_pair(outcommod, outFuel)));
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

