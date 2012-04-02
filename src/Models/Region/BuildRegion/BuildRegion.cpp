// BuildRegion.cpp
// Implements the BuildRegion class

#include "BuildRegion.h"

#include "InstModel.h"

#include <list>
#include <sstream>

#include "Timer.h"
#include "Logger.h"
#include "CycException.h"
#include "InputXML.h"

#include <iostream>

using namespace std;

/* --------------------
 * Comparison Functors
 * --------------------
 */
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
bool compare_order_times(PrototypeBuildOrder* o1, PrototypeBuildOrder* o2) {
  // sort by time
  return (o1->first < o2->first);
}

/* -------------------- */


/* --------------------
 * BuildRegion Class Methods
 * --------------------
 */
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void BuildRegion::init() {
  prototypeOrders_ = new PrototypeOrders();
  builders_ = new map<Model*, std::list<Model*>*>();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void BuildRegion::init(xmlNodePtr cur) {
  Model::init(cur); // name_ and model_impl_
  RegionModel::initAllowedFacilities(cur); // allowedFacilities_
  BuildRegion::init(); // initializes member vars
  
  // get path to this model
  xmlNodePtr model_cur = 
    XMLinput->get_xpath_element(cur,"model/BuildRegion");

  // populate orders for each prototype
  xmlNodeSetPtr prototype_nodes = 
    XMLinput->get_xpath_elements(model_cur,"prototyperequirement");
  for (int i=0;i<prototype_nodes->nodeNr;i++){
    populateOrders(prototype_nodes->nodeTab[i]);
  }
  sortOrders();
  
  RegionModel::init(); // parent_ and tick listener, model 'born'
  RegionModel::initChildren(cur); // children->setParent, requires init()
  
  // populate the list of builders
  populateBuilders();
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void BuildRegion::copy(BuildRegion* src) {
  RegionModel::copy(src);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void BuildRegion::populateOrders(xmlNodePtr cur) {  
  // get prototype
  string name = 
    (const char*)XMLinput->get_xpath_content(cur, "prototypename");
  Model* prototype = Model::getTemplateByName(name);
  
  // fill a set of orders with constructed orders
  PrototypeOrders* orders = new PrototypeOrders();

  // for each entry, get time and number and construct an order
  xmlNodeSetPtr entry_nodes = 
    XMLinput->get_xpath_elements(cur,"demandschedule");
  string sTime, sNumber;
  int time, number;
  for (int i=0;i<entry_nodes->nodeNr;i++){
    // get data
    xmlNodePtr entry_node = 
      XMLinput->get_xpath_element(entry_nodes->nodeTab[i],"entry");
    sTime = XMLinput->get_xpath_content(entry_node,"time");
    sNumber = XMLinput->get_xpath_content(entry_node,"number");
    time = strtol(sTime.c_str(),NULL,10);
    number = strtol(sNumber.c_str(),NULL,10);
    // construct
    PrototypeBuildOrder* order = new PrototypeBuildOrder();
    order = constructOrder(prototype,number,time);
    // fill
    orders->push_back(order);
  }
  populateOrders(orders);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
PrototypeBuildOrder* 
BuildRegion::constructOrder(Model* prototype, int number, int time) {
  PrototypeDemand d(prototype,number);
  PrototypeBuildOrder* b = new PrototypeBuildOrder(time,d);
  return b;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void BuildRegion::populateOrders(PrototypeOrders* orders) {
  for ( OrderIterator order = orders->begin(); 
        order != orders->end(); order++ ) {
    addOrder( (*order) );
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void BuildRegion::sortOrders() {
  prototypeOrders_->sort(compare_order_times);
  // error if first order is before the current time
  if ( frontOrder()->first < TI->time() ) {
    stringstream err("");
    err << "BuildRegion " << this->name() << " has sorted its orders, "
        << "and its first order has a timestamp PRECEDING the current "
        << "time.";
    throw CycOverrideException(err.str());    
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void BuildRegion::populateBuilders() {
  // for each child
  for(vector<Model*>::iterator inst = children_.begin();
      inst != children_.end(); inst++) {

    // for each prototype of that child
    for(PrototypeIterator fac = 
          (dynamic_cast<InstModel*>(*inst))->beginPrototype();
        fac != (dynamic_cast<InstModel*>(*inst))->endPrototype(); fac++) {
      
      // if fac not in builders_
      if ( builders_->find( (*fac) ) == builders_->end() ) {
        list<Model*>* builder_list = new list<Model*>();
        builder_list->push_back( (*inst) );
        builders_->insert( pair<Model*,list<Model*>*>( (*fac), builder_list) );
      }
      // if fac in builders_
      else {
        (*builders_)[(*fac)]->push_back( (*inst) );
      } // end builders_

    }  // end prototypes

  } // end children
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void BuildRegion::handleTick(int time) {
  // build something if an order has been placed
  if (!prototypeOrders_->empty()) {
    while (!prototypeOrders_->empty() && frontOrder()->first == time) {
      handlePrototypeOrder(frontOrder()->second);
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

/* -------------------- */


/* --------------------
 * Model Class Methods
 * --------------------
 */

extern "C" Model* constructBuildRegion() {
    return new BuildRegion();
}

/* -------------------- */

