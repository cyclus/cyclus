// FixedInst.cpp
// Implements the FixedInst class
#include <iostream>
#include "Logger.h"

#include "FixedInst.h"

#include "FacilityModel.h"

#include "Logician.h"
#include "CycException.h"
#include "InputXML.h"


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
void FixedInst::init(xmlNodePtr cur)
{
  InstModel::init(cur);

  /// get facility list
  xmlNodeSetPtr nodes = XMLinput->get_xpath_elements(cur,"model/FixedInst/facility");
  
  for (int i=0;i<nodes->nodeNr;i++) 
  {
    xmlNodePtr fac_node = nodes->nodeTab[i];
    string fac_name = XMLinput->get_xpath_content(fac_node,"type");
  
    Model* facility = LI->getModelByName(fac_name, FACILITY);

    
    if (!(dynamic_cast<RegionModel*>(region_))->isAllowedFacility(facility)){
      throw CycException("Facility '" 
                         + fac_name 
                         + "' is not an allowed facility for region '" 
                         + region_->getName() +"'.");
    }

    Model* new_facility = Model::create(facility);

    dynamic_cast<FacilityModel*>(new_facility)->setFacName(XMLinput->get_xpath_content(fac_node,"name"));
    dynamic_cast<FacilityModel*>(new_facility)->setInstName(this->getName());
    this->addFacility(new_facility);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
void FixedInst::copy(FixedInst* src)
{
  InstModel::copy(src);

  facilities_ = src->facilities_;

}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
void FixedInst::copyFreshModel(Model* src)
{

  copy(dynamic_cast<FixedInst*>(src));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
void FixedInst::print() 
{ 
  InstModel::print();

  LOG(LEV_DEBUG2) << " and the following permanent facilities: ";
  for (vector<Model*>::iterator fac=facilities_.begin(); 
       fac != facilities_.end(); 
       fac++){
    LOG(LEV_DEBUG2) << "        * " << (dynamic_cast<FacilityModel*>(*fac))->getFacName()
     << " (" << (*fac)->getName() << ")";
  }
};




