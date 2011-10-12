// DeployInst.cpp
// Implements the DeployInst class
#include <iostream>

#include "DeployInst.h"

#include "FacilityModel.h"

#include "Logician.h"
#include "GenException.h"
#include "InputXML.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
void DeployInst::init(xmlNodePtr cur)
{
  InstModel::init(cur);

  /// get deployments
  xmlNodeSetPtr nodes = XMLinput->get_xpath_elements(cur,"model/DeployInst/deploy");
  
  for (int i=0;i<nodes->nodeNr;i++) 
  {
    xmlNodePtr deploy = nodes->nodeTab[i];
    string fac_name = XMLinput->get_xpath_content(deploy,"type");
  
    Model* facility = LI->getModelByName(fac_name, FACILITY);

    if (NULL == facility){
      throw GenException("Facility '" 
                         + fac_name 
                         + "' is not defined in this problem.");
    }

    if (!(dynamic_cast<RegionModel*>(region_))->isAllowedFacility(facility)){
      throw GenException("Facility '" 
                         + fac_name 
                         + "' is not an allowed facility for region '" 
                         + region_->getName() +"'.");
    }
    //Model* new_facility = Model::create(facility);
    
    //dynamic_cast<FacilityModel*>(facility)->setFacName(XMLinput->get_xpath_content(deploy,"name"));
    
    int start_month = strtol(XMLinput->get_xpath_content(deploy,"start"), NULL, 10);
    
    if (start_month < 0){
      throw GenException("You can't deploy a facility in the past.");
    }
    deployment_map_[start_month] = facility;
  }
  to_build_map_ = deployment_map_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
void DeployInst::copy(DeployInst* src)
{
  InstModel::copy(src);

  deployment_map_ = src->deployment_map_;
  to_build_map_ = deployment_map_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
void DeployInst::copyFreshModel(Model* src)
{
  copy(dynamic_cast<DeployInst*>(src));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
void DeployInst::print() 
{ 
  InstModel::print();

  cout << " with deployment schedule: " << endl;

  for (map<int,Model*>::iterator deploy=deployment_map_.begin();
       deploy!=deployment_map_.end();
       deploy++){
    cout << "\t\t\tFacility " << dynamic_cast<FacilityModel*>((*deploy).second)->getFacName()
        << " ("  << (*deploy).second->getName() 
        << ") is deployed in month " << (*deploy).first << endl;
  }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
void DeployInst::handleTick(int time) {
  map<int,Model*>::iterator next_build = to_build_map_.begin();
  while (time==(*next_build).first) {
    Model* new_facility = Model::create((*next_build).second);
    //dynamic_cast<FacilityModel*>(new_facility)->setFacName(pointer, name);
    // this->addFacility((*next_build).second);
    to_build_map_.erase(next_build);
    next_build=to_build_map_.begin();
    LI->addModel(new_facility, FACILITY);
  };
};

