// Commodity.cpp
// Implements the Commodity Class
#include "Commodity.h"

#include "CycException.h"
#include "InputXML.h"

using namespace std;

/// Initialize the Commodity ID serialization
int Commodity::nextID_ = 0;
std::map<std::string, Commodity*> Commodity::commodities_;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Commodity::Commodity(std::string name) {
  ID_ = nextID_++;
  name_ = name;
  market_ = NULL;

  commodities_[name] = this;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Commodity::Commodity(xmlNodePtr cur) {
  ID_ = nextID_++;

  // get commodity name
  name_ = XMLinput->get_xpath_content(cur,"name");

  // don't give it any market
  market_ = NULL;

  commodities_[name_] = this;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Commodity::load_commodities() {
  xmlNodeSetPtr nodes = XMLinput->get_xpath_elements("/*/commodity");
  
  // Logician maintains a list of commods retrievable as needed by sim Agents
  for (int i=0;i<nodes->nodeNr;i++) {
    new Commodity(nodes->nodeTab[i]);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Model* Commodity::getMarket() {
  if (market_ == NULL) {
    string err_msg = "Attempted to retrieve an uninitialized market pointer";
    err_msg += "from a commodity.";
    throw CycNullException(err_msg);
  }

  return market_;
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int Commodity::getNumCommodities() {
  return Commodity::commodities_.size();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Commodity* Commodity::getCommodity(std::string name) {
  if (Commodity::commodities_.count(name) == 0) {
      throw CycNullException("Commodity '" + name 
          + "' does not exist.");
  }
  return Commodity::commodities_[name];
}
