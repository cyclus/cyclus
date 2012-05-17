// MarketModel.cpp
// Implements the MarketModel Class

#include <string>

#include "MarketModel.h"

#include "InputXML.h"
#include "CycException.h"
#include "Timer.h"
#include "Logger.h"

using namespace std;

list<MarketModel*> MarketModel::markets_;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
MarketModel::MarketModel() {
  init();
  setModelType("Market"); 
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
MarketModel::~MarketModel() {
  LOG(LEV_DEBUG2, "none!") << "removing market from static list of markets...";
  list<MarketModel*>::iterator mkt;
  for (mkt=markets_.begin(); mkt!=markets_.end(); ++mkt) {
    if (this == *mkt) {
      LOG(LEV_DEBUG2, "none!") << "  found match in static list";
      markets_.erase(mkt);
      LOG(LEV_DEBUG2, "none!") << "  match is removed";
      break;
    }
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
MarketModel* MarketModel::marketForCommod(std::string commod) {
  MarketModel* market = NULL;
  list<MarketModel*>::iterator mkt;
  for (mkt=markets_.begin(); mkt!=markets_.end(); ++mkt){
    if ((*mkt)->commodity() == commod) {
      market = *mkt;
      break;
    }
  }

  if (market == NULL) {
    string err_msg = "No market found for commodity '";
    err_msg += commod + "'.";
    throw CycIndexException(err_msg);
  }
  return market;
}
 
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void MarketModel::registerMarket(MarketModel* mkt) {
  markets_.push_back(mkt);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void MarketModel::initSimInteraction(MarketModel* mkt) {  
  // this brings the market into the simulation (all agents must have a parent)
  mkt->setParent(mkt);

  // register the model
  TI->registerResolveListener(mkt);
  MarketModel::registerMarket(mkt);
}
  
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void MarketModel::init(xmlNodePtr cur) {
  // general initializations
  Model::init(cur);  
  MarketModel::initSimInteraction(this);

  // specific initalizations
  commodity_ = XMLinput->get_xpath_content(cur,"mktcommodity");
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void MarketModel::copy(MarketModel* src) {
  Model::copy(src);
  Communicator::copy(src);

   /** 
   *  Specific initialization for MarketModels
   */

  commodity_ = src->commodity();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
std::string MarketModel::str() { 
  return Model::str() + " trades commodity " + commodity_; 
};

