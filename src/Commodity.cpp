// Commodity.cpp
// Implements the Commodity Class
#include <string>

#include "Commodity.h"

#include "Logician.h"
#include "GenException.h"
#include "InputXML.h"

/**
 *  Initialize the Commodity ID serialization
 */
int Commodity::nextID = 0;

Commodity::Commodity(xmlNodePtr cur)
{
    ID = nextID++;

    name = XMLinput->get_xpath_content(cur,"name");

    market = NULL;

}

void Commodity::load_commodities()
{
    xmlNodeSetPtr nodes = XMLinput->get_xpath_elements("/*/commodity");
    
    for (int i=0;i<nodes->nodeNr;i++)
	LI->addCommodity(new Commodity(nodes->nodeTab[i]));
}

