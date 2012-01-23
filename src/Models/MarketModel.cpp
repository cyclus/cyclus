// MarketModel.cpp
// Implements the MarketModel Class

#include "MarketModel.h"
#include "InputXML.h"
#include "CycException.h"
#include "Timer.h"

#include "Logger.h"
#include <string>

using namespace std;

list<MarketModel*> MarketModel::markets_;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
MarketModel::MarketModel() {
  setModelType("Market");

  // register the model
  this->registerModel();

  TI->registerResolveListener(this);
  markets_.push_back(this);

};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
MarketModel::~MarketModel() {
  LOG(LEV_DEBUG2) << "removing market from static list of markets...";
  std::list<MarketModel*>::iterator mkt;
  for (mkt=markets_.begin(); mkt!=markets_.end(); ++mkt) {
    if (this == *mkt) {
      LOG(LEV_DEBUG2) << "  found match in static list";
      markets_.erase(mkt);
      LOG(LEV_DEBUG2) << "  match is removed";
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
    throw CycIndexException(err_msg);
  }
  return market;
}
  
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void MarketModel::init(xmlNodePtr cur) {
  Model::init(cur);
  
  /** 
   *  Specific initialization for MarketModels
   */

  /// all markets require commodities
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
void MarketModel::print() { 
  Model::print(); 

  LOG(LEV_DEBUG2) << "    trades commodity " << commodity_;

};

