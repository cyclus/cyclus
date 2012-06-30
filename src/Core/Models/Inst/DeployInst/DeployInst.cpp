// DeployInst.cpp
// Implements the DeployInst class
#include <iostream>
#include <sstream>

#include "DeployInst.h"

#include "FacilityModel.h"

#include "Logger.h"
#include "CycException.h"
#include "InputXML.h"

using namespace std;

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
  
    Model* facility = Model::getTemplateByName(fac_name);

    if (!(dynamic_cast<RegionModel*>( parent() ))->isAllowedFacility(facility)){
      string err_msg = "Facility '" + fac_name;
      err_msg += "' is not an allowed facility for region '";
      err_msg +=  parent()->name() +"'.";
      throw CycException(err_msg);
    }
    //Model* new_facility = Model::create(facility);
    
    //dynamic_cast<FacilityModel*>(facility)->setFacName(XMLinput->get_xpath_content(deploy,"name"));
    
    int start_month = strtol(XMLinput->get_xpath_content(deploy,"start"), NULL, 10);
    
    if (start_month < 0){
      throw CycRangeException("You can't deploy a facility in the past.");
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
std::string DeployInst::str() { 
  std::stringstream ss;
  ss << InstModel::str()
     << " with deployment schedule: ";

  for (map<int,Model*>::iterator deploy=deployment_map_.begin();
       deploy!=deployment_map_.end();
       deploy++){
     ss << " Facility " << dynamic_cast<FacilityModel*>((*deploy).second)->facName()
        << " ("  << (*deploy).second->name() 
        << ") deployed month " << (*deploy).first
        << ", ";
  }
  return ss.str() + '.';
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
  };
};

/* ------------------- */ 


/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

extern "C" Model* constructDeployInst() {
  return new DeployInst();
}

/* ------------------- */ 

