// StubMarket.cpp
// Implements the StubMarket class
#include <iostream>

#include "StubMarket.h"


/* --------------------
 * all MODEL classes have these members
 * --------------------
 */
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
StubMarket::StubMarket() {};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
StubMarket::~StubMarket() {};
    
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void StubMarket::init(xmlNodePtr cur) { 
  MarketModel::init(cur); 
  this->init(member_var_map_);
};
    
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void StubMarket::init(std::map<std::string, void*> member_var_map) { 
  MarketModel::init(member_var_map); 
};
  
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void StubMarket::copy(StubMarket* src) { 
  MarketModel::copy(src); 
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void StubMarket::copyFreshModel(Model* src){
  copy(dynamic_cast<StubMarket*>(src));
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void StubMarket::print() { 
  MarketModel::print(); 
};

/* --------------------
 * all COMMUNICATOR classes have these members
 * --------------------
 */
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void StubMarket::sendMessage() {};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void StubMarket::receiveMessage(Message* msg) {};


/* --------------------
 * all MARKETMODEL classes have these members
 * --------------------
 */

void StubMarket::resolve() {};

