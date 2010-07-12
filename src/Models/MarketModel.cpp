// MarketModel.cpp
// Implements the MarketModel Class

#include "MarketModel.h"

#include "InputXML.h"
#include "GenException.h"
#include "Logician.h"

/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

// Initializes the MarketModel nextID to 0.
int MarketModel::nextID = 0;

MarketModel::MarketModel(xmlNodePtr cur)
{
    /** 
     *  Generic initialization for Models
     */

    ID = nextID++;

    name = XMLinput->get_xpath_content(cur,"name");

     /** 
     *  Specific initialization for MarketModels
     */

    /// all markets require commodities
    string commod_name = XMLinput->get_xpath_content(cur,"mktcommodity");
    commodity = LI->getCommodity(commod_name);
    if (NULL == commodity)
	throw GenException("That market commodity does not exist.");

    commodity->setMarket(this);
}


/* --------------------
 * all MARKETMODEL classes have these members
 * --------------------
 */
