// FixedInst.cpp
// Implements the FixedInst class
#include <iostream>

#include "FixedInst.h"

#include "FacilityModel.h"

#include "Timer.h"
#include "Logger.h"
#include "CycException.h"
#include "InputXML.h"

using namespace std;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
void FixedInst::init(xmlNodePtr cur) {
  InstModel::init(cur);

  /// get facility list
  xmlNodeSetPtr nodes = XMLinput->get_xpath_elements(cur,"model/FixedInst/facility");
  
  for (int i = 0; i<nodes->nodeNr; i++) {
    xmlNodePtr fac_node = nodes->nodeTab[i];
    string fac_name = XMLinput->get_xpath_content(fac_node,"type");
  
    Model* facility = Model::getTemplateByName(fac_name);
    facility->setName(XMLinput->get_xpath_content(fac_node,"name"));
    facilities_.push(facility);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
void FixedInst::copy(FixedInst* src) {
  InstModel::copy(src);
  facilities_ = src->facilities();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
void FixedInst::copyFreshModel(Model* src) {
  copy(dynamic_cast<FixedInst*>(src));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
void FixedInst::print() {
  InstModel::print();
  if (children_.size() > 0) {
    LOG(LEV_DEBUG2, "none!") << "has the following permanent facilities: ";
    for (vector<Model*>::iterator fac=children_.begin(); 
         fac != children_.end(); 
         fac++){
      LOG(LEV_DEBUG2, "none!") << "  * " << (dynamic_cast<FacilityModel*>(*fac))->facName()
                               << " (" << (*fac)->name() << ")";
    }
  }
  else {
    LOG(LEV_DEBUG2, "none!") << "has no built facilities (currently).";
  }
};

void FixedInst::handleTick(int time){
  // if time is t0, create those facilities
  if ( time == TI->startTime() ) {
    while (facilities_.size() > 0) {
      Model* facility = facilities_.front();
      // check that the facility is in the allowed facilities
      if (!(dynamic_cast<RegionModel*>( parent() ))->isAllowedFacility(facility)){
        throw CycException("Facility '" 
                           + facility->name() 
                           + "' is not an allowed facility for region '" 
                           + parent()->name() +"'.");
      }
      // if its allowed, build it
      Model* new_facility = Model::create(facility);
      new_facility->setName(facility->name());
      new_facility->setParent(this);
      facilities_.pop();
    }
  }
  // in any case, send children the handle ticks
  InstModel::handleTick(time);
}

/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

extern "C" Model* constructFixedInst() {
  return new FixedInst();
}

/* ------------------- */ 



