// BuildRegion.cpp
// Implements the BuildRegion class

#include "BuildRegion.h"

#include "InstModel.h"

#include <list>
// #include <utility>
//#include <set>
#include <sstream>

#include "Logger.h"
#include "CycException.h"
#include "InputXML.h"

using namespace std;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void BuildRegion::init(xmlNodePtr cur) {
  RegionModel::init(cur);
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void BuildRegion::copy(BuildRegion* src) {
  RegionModel::copy(src);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void BuildRegion::handleTick(int time) {
  // build something if an order has been placed
  if (!prototypeOrders_->empty()) {
    while (prototypeOrders_->front().first == time) {
      handlePrototypeOrder(prototypeOrders_->front().second);
      prototypeOrders_->pop_front();
    }
  }
  // After we finish building, call the normal handleTick for a region
  RegionModel::handleTick(time);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void BuildRegion::handlePrototypeOrder(PrototypeDemand order) {
  Model* prototype = order.first;
  int nTotal = order.second;
  for (int i = 0; i < nTotal; i++) {
    list<ModelIterator> bidders;
    getAvailableBuilders(prototype,bidders);
    Model* builder = selectBuilder(prototype,bidders);
    placeOrder(prototype,builder);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void BuildRegion::getAvailableBuilders(Model* prototype, 
                                       list<ModelIterator>& bidders) {
  for(ModelIterator child = (*builders_)[prototype]->begin();
      child != (*builders_)[prototype]->end(); 
      child++) {
    if ( (*child)->canBuild(prototype) ){
      bidders.push_back(child);
    }
  }
  // if there are no bidders, throw an error for now
  if ( bidders.empty() ) {
    stringstream err("");
    err << "BuildRegion " << this->name() << " is attempting to build "
        << prototype->name() << " but none of its children are "
        << "available to build it.";
    throw CycOverrideException(err.str());
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
Model* BuildRegion::selectBuilder(Model* prototype, 
                                  list<ModelIterator>& bidders) {
  // highest bidder guaranteed to be front -- see header file
  ModelIterator builder = bidders.front();
  // move builder to end of list
  (*builders_)[prototype]->splice( (*builders_)[prototype]->end(), 
                                   (*(*builders_)[prototype]), builder);
  return (*builder);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void BuildRegion::placeOrder(Model* prototype, Model* builder) {
  // build functions must know who is placing the build order
  dynamic_cast<InstModel*>(builder)->build(prototype,this);
}


/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

extern "C" Model* constructBuildRegion() {
    return new BuildRegion();
}


/* -------------------- */
