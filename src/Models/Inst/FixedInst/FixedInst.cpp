// FixedInst.cpp
// Implements the FixedInst class
#include <iostream>

#include "FixedInst.h"

#include "FacilityModel.h"

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
    
    if (!(dynamic_cast<RegionModel*>( parent() ))->isAllowedFacility(facility)){
      throw CycException("Facility '" 
                         + fac_name 
                         + "' is not an allowed facility for region '" 
                         + parent()->name() +"'.");
    }

    Model* new_facility = Model::create(facility);

    new_facility->setName(XMLinput->get_xpath_content(fac_node,"name"));
    new_facility->setParent(this);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
void FixedInst::copy(FixedInst* src) {
  InstModel::copy(src);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
void FixedInst::copyFreshModel(Model* src) {
  copy(dynamic_cast<FixedInst*>(src));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
void FixedInst::print() {
  InstModel::print();

  LOG(LEV_DEBUG2, "none!") << " and the following permanent facilities: ";
  for (vector<Model*>::iterator fac=children_.begin(); 
       fac != children_.end(); 
       fac++){
    LOG(LEV_DEBUG2, "none!") << "        * " << (dynamic_cast<FacilityModel*>(*fac))->facName()
     << " (" << (*fac)->name() << ")";
  }
};

/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

extern "C" Model* constructFixedInst() {
  return new FixedInst();
}

/* ------------------- */ 



