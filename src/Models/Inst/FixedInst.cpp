// FixedInst.cpp
// Implements the FixedInst class
#include <iostream>

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

    
    if (!(dynamic_cast<RegionModel*>( parent() ))->isAllowedFacility(facility)){
      throw CycException("Facility '" 
                         + fac_name 
                         + "' is not an allowed facility for region '" 
                         + parent()->getName() +"'.");
    }

    Model* new_facility = Model::create(facility);

    dynamic_cast<FacilityModel*>(new_facility)->setFacName(XMLinput->get_xpath_content(fac_node,"name"));
    this->addChild(new_facility);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
void FixedInst::copy(FixedInst* src)
{
  InstModel::copy(src);

  children_ = src->children_;

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

  cout << " and the following permanent facilities: " << endl;
  for (vector<Model*>::iterator fac=children_.begin(); 
       fac != children_.end(); 
       fac++){
    cout << "\t\t* " << (dynamic_cast<FacilityModel*>(*fac))->getFacName()
     << " (" << (*fac)->getName() << ")" << endl;
  }
};




