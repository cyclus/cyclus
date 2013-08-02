// MarketModel.cpp
// Implements the MarketModel Class

#include <string>

#include "MarketModel.h"

#include "Timer.h"
#include "Logger.h"
#include "QueryEngine.h"

namespace cyclus {

std::list<MarketModel*> MarketModel::markets_;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
MarketModel::MarketModel() {
  setModelType("Market"); 
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
MarketModel::~MarketModel() {
  LOG(LEV_DEBUG2, "none!") << "removing market from static list of markets...";
  std::list<MarketModel*>::iterator mkt;
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
  std::list<MarketModel*>::iterator mkt;
  for (mkt=markets_.begin(); mkt!=markets_.end(); ++mkt){
    if ((*mkt)->commodity() == commod) {
      market = *mkt;
      break;
    }
  }

  if (market == NULL) {
    std::string err_msg = "No market found for commodity '";
    err_msg += commod + "'.";
    throw CycMarketlessCommodException(err_msg);
  }
  return market;
}
 
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void MarketModel::registerMarket(MarketModel* mkt) {
  markets_.push_back(mkt);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void MarketModel::enterSimulationAsCoreEntity() {
  // register the model
  TI->registerResolveListener(this);
  MarketModel::registerMarket(this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void MarketModel::setCommodity(std::string name) {
  commodity_ = name;
}
  
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void MarketModel::initCoreMembers(QueryEngine* qe) {
  // general initializations
  Model::initCoreMembers(qe);

  // specific initalizations
  setCommodity(qe->getElementContent("mktcommodity"));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
std::string MarketModel::str() { 
  return Model::str() + " trades commodity " + commodity_; 
};
} // namespace cyclus
