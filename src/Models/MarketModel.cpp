// MarketModel.cpp
// Implements the MarketModel Class

#include "MarketModel.h"
#include "InputXML.h"
#include "CycException.h"
#include "Timer.h"

#include "Logger.h"
#include <string>

using namespace std;

std::vector<MarketModel*> MarketModel::markets_;

MarketModel::MarketModel() {
  setModelType("Market");

  TI->registerResolveListener(this);
  markets_.push_back(this);

};

MarketModel* MarketModel::marketForCommod(std::string commod) {
  MarketModel* market = NULL;
  for (int i = 0; i < markets_.size(); i++) {
    if (markets_.at(i)->commodity() == commod) {
      market = markets_.at(i);
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
  
void MarketModel::init(xmlNodePtr cur) {
  Model::init(cur);
  
  /** 
   *  Specific initialization for MarketModels
   */

  /// all markets require commodities
  commodity_ = XMLinput->get_xpath_content(cur,"mktcommodity");
}

void MarketModel::copy(MarketModel* src) {
  Model::copy(src);
  Communicator::copy(src);

   /** 
   *  Specific initialization for MarketModels
   */

  commodity_ = src->commodity();
}

void MarketModel::print() { 
  Model::print(); 

  LOG(LEV_DEBUG2) << "    trades commodity " << commodity_;

};

