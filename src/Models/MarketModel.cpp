// MarketModel.cpp
// Implements the MarketModel Class

#include "MarketModel.h"

/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

// Initializes the MarketModel nextID to 0.
int MarketModel::nextID = 0;

#include "InputXML.h"
#include "GenException.h"
#include "Logician.h"

void MarketModel::init(xmlNodePtr cur)
{
  Model::init(cur);
  
  /** 
   *  Specific initialization for MarketModels
   */

  /// all markets require commodities
  string commod_name = XMLinput->get_xpath_content(cur,"mktcommodity");
  commodity = LI->getCommodity(commod_name);
  if (NULL == commodity)
    throw GenException("Market commodity '" + commod_name 
        + "' does not exist for market '" + getName() 
        + "'.");
  
  commodity->setMarket(this);
}

void MarketModel::copy(MarketModel* src)
{
  Model::copy(src);
  Communicator::copy(src);

   /** 
   *  Specific initialization for MarketModels
   */

  commodity = src->commodity;
  LI->addMarket(this);

}


void MarketModel::print()              
{ 
  Model::print(); 

  cout << "trades commodity " 
      << commodity->getName() << endl;

} ;


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

  while( orders.size() > 0)
  {
    (*orders.begin())->execute();
    orders.pop_front();
  }


}
