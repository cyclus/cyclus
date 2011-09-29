// Commodity.cpp
// Implements the Commodity Class
#include "Commodity.h"

#include "Logician.h"
#include "GenException.h"
#include "InputXML.h"

/// Initialize the Commodity ID serialization
int Commodity::nextID_ = 0;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Commodity::Commodity(xmlNodePtr cur) {
  // advance the commodity ID
  ID_ = nextID_++;

  // get commodity name
  name_ = XMLinput->get_xpath_content(cur,"name");

  // don't give it any market
  market_ = NULL;

}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Commodity::load_commodities() {
  xmlNodeSetPtr nodes = XMLinput->get_xpath_elements("/*/commodity");
  
  // Logician maintains a list of commods retrievable as needed by sim Agents
  for (int i=0;i<nodes->nodeNr;i++) {
    LI->addCommodity(new Commodity(nodes->nodeTab[i]));
  }
}

