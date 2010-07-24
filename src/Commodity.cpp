// Commodity.cpp
// Implements the Commodity Class
#include <string>

#include "Commodity.h"

#include "Logician.h"
#include "GenException.h"
#include "InputXML.h"

/// Initialize the Commodity ID serialization
int Commodity::nextID = 0;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Commodity::Commodity(xmlNodePtr cur)
{
  // advance the commodity ID
  ID = nextID++;

  // get the name of the commodity
  name = XMLinput->get_xpath_content(cur,"name");

  // don't give it any market
  market = NULL;

}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Commodity::load_commodities()
{
  xmlNodeSetPtr nodes = XMLinput->get_xpath_elements("/*/commodity");
  
  // for each node from the input, add a commodity to the Logician's list.
  for (int i=0;i<nodes->nodeNr;i++)
    LI->addCommodity(new Commodity(nodes->nodeTab[i]));
}

