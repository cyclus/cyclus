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
void BuildInst::init() {
  totalBuildCount_ = 0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void BuildInst::init(xmlNodePtr cur) {
  // xml inits
  InstModel::init(cur);

  LOG(LEV_DEBUG2, "binst") << "A Build Inst is being initialized";

  // get path to this model
  xmlNodePtr model_cur = 
    XMLinput->get_xpath_element(cur,"model/BuildInst");

  // this institution must have a list of available prototypes
  xmlNodeSetPtr nodes = 
    XMLinput->get_xpath_elements(model_cur,"availableprototype");

  // populate prototypes_
  string name;
  Model* prototype;
  for (int i=0;i<nodes->nodeNr;i++){
    name = (const char*)nodes->nodeTab[i]->children->content;
    prototype = Model::getTemplateByName(name);
    prototypes_->insert(prototype);
  }

  // yell if there are no prototypes
  if ( prototypes_->empty() ) {
    stringstream err("");
    err << "BuildInst " << this->name() << " cannot be initiated "
        << "with no available prototypes!";
    throw CycOverrideException(err.str());
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void BuildInst::copy(BuildInst* src) {
  InstModel::copy(src);

  prototypes_ = src->prototypes_;
  totalBuildCount_ = 0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
std::string BuildInst::str() {
  std::string s = InstModel::str();

  if ( prototypes_ == NULL || prototypes_->empty() ){
    s += name() + " has no prototypes (currently)."; 
  } else {
    s += name() + " has prototypes: ";
    for (set<Model*>::iterator mdl=prototypes_->begin(); 
         mdl != prototypes_->end(); 
         mdl++){
      s += (*mdl)->name() + ", ";
    }
  }
  return s;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void BuildInst::doBuild(Model* prototype, std::string name) {
  Model* new_facility = Model::create(prototype);
  new_facility->setName(name);
  new_facility->setParent(this);
  totalBuildCount_++;
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
void BuildInst::build(Model* prototype, Model* requester, std::string name) {
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

