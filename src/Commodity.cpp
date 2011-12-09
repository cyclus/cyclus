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
Model* Commodity::getMarket() {
  if (market_ == NULL) {
    string err_msg = "Attempted to retrieve an uninitialized market pointer";
    err_msg += "from a commodity.";
    throw CycIndexException(err_msg);
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
      throw CycIndexException("Commodity '" + name 
          + "' does not exist.");
  }
  return Commodity::commodities_[name];
}
