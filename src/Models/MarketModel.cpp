// MarketModel.cpp
// Implements the MarketModel Class

#include "MarketModel.h"

#include "InputXML.h"
#include "GenException.h"
#include "Logician.h"

/* --------------------
 * all MODEL classes have these public members
 * --------------------
 */

// Initializes the MarketModel nextID to 0.
int MarketModel::nextID = 0;

MarketModel::MarketModel(xmlNodePtr cur)
{
    ID = nextID++;

    name = XMLinput->get_child_content(cur,"name");

    /// all markets require commodities
    string commod_name = XMLinput->get_child_content(cur,"mktcommodity");
    commodity = LI->getCommodity(commod_name);
    commodity->setMarket(this);
}


/* --------------------
 * all MARKETMODEL classes have these protected members
 * --------------------
 */
