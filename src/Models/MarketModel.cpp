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

    name = (const char*)xmlGetProp(cur, (const xmlChar*)"name");

    /// all markets require commodities

    xmlNodeSetPtr nodes = XMLinput->get_elements(cur,"Commodity");

    if (nodes->nodeNr != 1)
	throw GenException("Must be exactly 1 Commodity per Market, found ");
    
    string commod_name = (const char*)xmlGetProp(nodes->nodeTab[0], 
						 (const xmlChar*)"name");

    commodity = LI->getCommodity(commod_name);
}


/* --------------------
 * all MARKETMODEL classes have these protected members
 * --------------------
 */
