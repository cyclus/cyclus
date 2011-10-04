// FixedInst.cpp
// Implements the FixedInst class
#include <iostream>

#include "FixedInst.h"

#include "FacilityModel.h"

#include "Logician.h"
#include "GenException.h"
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
    // TYPE
    string fac_type = XMLinput->get_xpath_content(fac_node,"type");
    // NAME
    string fac_name = XMLinput->get_xpath_content(fac_node,"name");
    fac_list_.push_back(make_pair(fac_type, fac_name));
  }
  setMapVar("fac_list_",&fac_list_);
  this->init(member_var_map_);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
void FixedInst::init(map<string, void*> member_var_map)
{
  member_var_map_ = member_var_map;
  InstModel::init(member_var_map);
  // for each facility in the list 
  fac_list_ = getMapVar<deque< pair< string, string > > >("fac_list_",member_var_map);
  for(deque< pair< string , string> >::iterator iter = fac_list_.begin();
        iter != fac_list_.end();
        iter++){
    // get the implementation of the model
    string fac_type = (*iter).first;
    // create a facility pointer to that model
    Model* facility = LI->getModelByName(fac_type, FACILITY);
    // freak out if it isn't in the logician already
    if (NULL == facility){
      throw GenException("Facility '" 
                         + fac_type 
                         + "' is not defined in this problem.");
    }
    // if this region doesn't allow this facility, also freak out 
    if (!(dynamic_cast<RegionModel*>(region_))->isAllowedFacility(facility)){
      throw GenException("Facility '" 
                         + fac_type 
                         + "' is not an allowed facility for region '" 
                         + region_->getName() +"'.");
    }
    // now, create the new facility from the pointer
    Model* new_facility = Model::create(facility);
    // you'll need to set its name
    dynamic_cast<FacilityModel*>(new_facility)->setFacName((*iter).second);
    // as well as its inst name
    dynamic_cast<FacilityModel*>(new_facility)->setInstName(this->getName());
    // and register it with the logician
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

  cout << " and the following permanent facilities: " << endl;
  for (vector<Model*>::iterator fac=facilities_.begin(); 
       fac != facilities_.end(); 
       fac++){
    cout << "\t\t* " << (dynamic_cast<FacilityModel*>(*fac))->getFacName()
     << " (" << (*fac)->getName() << ")" << endl;
  }
};




