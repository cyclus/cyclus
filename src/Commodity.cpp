// Commodity.cpp
// Implements the Commodity Class
#include <string>

#include <stdlib.h>

#include "Logician.h"
#include "Commodity.h"
#include "GenException.h"

#include "InputXML.h"

/// Initialize the commodity ID serialization
int Commodity::nextID = 0;

Commodity::Commodity(xmlNodePtr cur)
{
    ID = nextID++;

    name = XMLinput->get_xpath_content(cur,"name");

    market = NULL;

}

Commodity::~Commodity(){
	// Delete any commodity data members that remain even when 
	// we go out of Commodity scope... unless you want to write 
	// that info to the database.
}

void Commodity::load_commodities()
{
    xmlNodeSetPtr nodes = XMLinput->get_xpath_elements("/simulation/commodity");
    
    if (!nodes)
	throw GenException("No Commodities defined in this simulation.");
    
    for (int i=0;i<nodes->nodeNr;i++)
	LI->addCommodity(new Commodity(nodes->nodeTab[i]));
}

