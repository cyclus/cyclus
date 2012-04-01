// BuildInst.cpp
// Implements the BuildInst class

#include <iostream>
#include <string>
#include <sstream>

#include "BuildInst.h"

#include "FacilityModel.h"

#include "Logger.h"
#include "CycException.h"
#include "InputXML.h"

using namespace std;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void BuildInst::init(xmlNodePtr cur) {
  InstModel::init(cur);

  // this institution must have a list of available prototypes
  xmlNodeSetPtr nodes = 
    XMLinput->get_xpath_elements(cur,"availableprototypes");

  string name;
  Model* prototype;
  
  for (int i=0;i<nodes->nodeNr;i++){
    name = (const char*)nodes->nodeTab[i]->children->content;
    prototype = Model::getTemplateByName(name);
    prototypes_.insert(prototype);
  }
  totalBuildCount_ = 0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void BuildInst::copy(BuildInst* src) {
  InstModel::copy(src);

  prototypes_ = src->prototypes_;
  totalBuildCount_ = 0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void BuildInst::copyFreshModel(Model* src) {
  copy(dynamic_cast<BuildInst*>(src));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void BuildInst::print() {
  InstModel::print();

  LOG(LEV_DEBUG2, "none!") 
    << " and the following available prototypes: ";
  for (set<Model*>::iterator mdl=prototypes_.begin(); 
       mdl != prototypes_.end(); 
       mdl++){
    LOG(LEV_DEBUG2, "none!") << "        * " << (*mdl)->name();
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void BuildInst::doBuild(Model* prototype, string name) {
  Model* new_facility = Model::create(prototype);
  new_facility->setName(name);
  new_facility->setParent(this);
  totalBuildCount_++;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void BuildInst::addPrototype(Model* prototype) {
  if ( !isAvailablePrototype(prototype) ) {
    prototypes_.insert(prototype);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void BuildInst::build(Model* prototype, Model* requester) {
  // set an arbitrary name
  stringstream name("");
  name << prototype->name() << totalBuildCount_;
  // call the full build function
  this->build(prototype, requester, name.str());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void BuildInst::build(Model* prototype, Model* requester, string name) {
  if ( requester != this->parent() ) {
    // if the requester is not this inst's parent, throw an error
    stringstream err("");
    err << "Model " << requester->name() << " is requesting that "
        << "BuildInst " << this->name() << " build a prototype, but is "
        <<"not the BuildInst's parent.";
    throw CycOverrideException(err.str());
  }
  else if ( !isAvailablePrototype(prototype) ) {
    // if the prototype is not in the set of available prototypes, 
    // throw an error
    stringstream err("");
    err << "Model " << requester->name() << " is requesting that "
        <<"BuildInst " << this->name() << " build a prototype of type " 
        << prototype->name() 
  	<< " but that prototype is not available via this BuildInst.";
    throw CycOverrideException(err.str());
  }
  else {
    // if all's good, build the prototype
    doBuild(prototype, name);
  }
}


/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

extern "C" Model* constructBuildInst() {
  return new BuildInst();
}

/* ------------------- */ 

