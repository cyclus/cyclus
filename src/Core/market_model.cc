// market_model.cc
// Implements the MarketModel Class

#include "market_model.h"

#include <string>

#include "timer.h"
#include "logger.h"
#include "error.h"

namespace cyclus {

std::list<MarketModel*> MarketModel::markets_;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MarketModel::MarketModel(Context* ctx) : Model(ctx) {
  SetModelType("Market");
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MarketModel::~MarketModel() {
  LOG(LEV_DEBUG2, "none!") << "removing market from static list of markets...";
  std::list<MarketModel*>::iterator mkt;
  for (mkt = markets_.begin(); mkt != markets_.end(); ++mkt) {
    if (this == *mkt) {
      LOG(LEV_DEBUG2, "none!") << "  found match in static list";
      markets_.erase(mkt);
      LOG(LEV_DEBUG2, "none!") << "  match is removed";
      break;
    }
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MarketModel* MarketModel::MarketForCommod(std::string commod) {
  MarketModel* market = NULL;
  std::list<MarketModel*>::iterator mkt;
  for (mkt = markets_.begin(); mkt != markets_.end(); ++mkt) {
    if ((*mkt)->commodity() == commod) {
      market = *mkt;
      break;
    }
  }

  if (market == NULL) {
    std::string err_msg = "No market found for commodity '";
    err_msg += commod + "'.";
    throw KeyError(err_msg);
  }
  return market;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void MarketModel::RegisterMarket(MarketModel* mkt) {
  markets_.push_back(mkt);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void MarketModel::EnterSimulationAsCoreEntity() {
  // register the model
  context()->RegisterResolver(this);
  MarketModel::RegisterMarket(this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void MarketModel::SetCommodity(std::string name) {
  commodity_ = name;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void MarketModel::InitCoreMembers(QueryEngine* qe) {
  // general initializations
  Model::InitCoreMembers(qe);

  // specific initalizations
  SetCommodity(qe->GetElementContent("mktcommodity"));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::string MarketModel::str() {
  return Model::str() + " trades commodity " + commodity_;
};
} // namespace cyclus
