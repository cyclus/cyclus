// MarketModel.cpp
// Implements the MarketModel Class

#include "MarketModel.h"
#include "InputXML.h"
#include "CycException.h"
#include "Logician.h"
#include "Timer.h"

#include <iostream>
#include <string>

using namespace std;

MarketModel::MarketModel() {
  setModelType("Market");
  TI->registerResolveListener(this);
};
  
void MarketModel::init(xmlNodePtr cur)
{
  Model::init(cur);
  
  /** 
   *  Specific initialization for MarketModels
   */

  /// all markets require commodities
  string commod_name = XMLinput->get_xpath_content(cur,"mktcommodity");
  commodity_ = LI->getCommodity(commod_name);
  if (NULL == commodity_)
    throw CycException("Market commodity '" + commod_name 
        + "' does not exist for market '" + getName() 
        + "'.");
  
  commodity_->setMarket(this);
}

void MarketModel::copy(MarketModel* src) {
  Model::copy(src);
  Communicator::copy(src);

   /** 
   *  Specific initialization for MarketModels
   */

  commodity_ = src->commodity_;
  LI->addModel(this, MARKET);
}

void MarketModel::print() { 
  Model::print(); 

  cout << "trades commodity " 
      << commodity_->getName() << endl;

};

/* --------------------
 * all COMMUNICATOR classes have these members
 * --------------------
 */


/* --------------------
 * all MARKETMODEL classes have these members
 * --------------------
 */

void MarketModel::executeOrderQueue()
{

  while( orders_.size() > 0)
  {
    (*orders_.begin())->execute();
    orders_.pop_front();
  }


}
