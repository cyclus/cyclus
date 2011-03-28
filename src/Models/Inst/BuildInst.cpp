// BuildInst.cpp
// Implements the BuildInst class
#include <iostream>

#include "BuildInst.h"

#include "FacilityModel.h"
#include "Model.h"

#include "Logician.h"
#include "GenException.h"
#include "InputXML.h"


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
void BuildInst::init(xmlNodePtr cur)
{
  InstModel::init(cur);

  /// get facility list
  xmlNodeSetPtr nodes = XMLinput->get_xpath_elements(cur,"model/BuildInst/facility");
  
  for (int i=0;i<nodes->nodeNr;i++) 
  {
    xmlNodePtr fac_node = nodes->nodeTab[i];
    string fac_name = XMLinput->get_xpath_content(fac_node,"type");
  
    Model* facility = LI->getFacilityByName(fac_name);

    if (NULL == facility){
      throw GenException("Facility '" 
                         + fac_name 
                         + "' is not defined in this problem.");
    }
    
    if (!((RegionModel*)region)->isAllowedFacility(facility)){
      throw GenException("Facility '" 
                         + fac_name 
                         + "' is not an allowed facility for region '" 
                         + region->getName() +"'.");
  }

    Model* new_facility = Model::create(facility);

    ((FacilityModel*)new_facility)->setFacName(XMLinput->get_xpath_content(fac_node,"name"));
    ((FacilityModel*)new_facility)->setInstName(this->getName());
    this->addFacility(new_facility);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
void BuildInst::copy(BuildInst* src)
{
  InstModel::copy(src);

  facilities = src->facilities;

}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
void BuildInst::copyFreshModel(Model* src)
{

  copy((BuildInst*)src);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
void BuildInst::print() 
{ 
  InstModel::print();

  cout << " and the following permanent facilities: " << endl;
  for (vector<Model*>::iterator fac=facilities.begin(); 
       fac != facilities.end(); 
       fac++){
    cout << "\t\t* " << ((FacilityModel*)(*fac))->getFacName()
     << " (" << (*fac)->getName() << ")" << endl;
  }
};


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void BuildInst::pleaseBuild(Model* fac)
{
  Model* new_facility=Model::create(fac);
  string name = ((FacilityModel*)fac)->getFacName()+" 2";
  ((FacilityModel*)new_facility)->setFacName(name);
  ((FacilityModel*)new_facility)->setInstName(this->getName());
  this->addFacility(new_facility);
  std::cout << "INST CREATES NEW FAC " << name << std::endl;
};

