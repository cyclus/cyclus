// DeployInst.cpp
// Implements the DeployInst class
#include <iostream>

#include "DeployInst.h"

#include "FacilityModel.h"

#include "Logician.h"
#include "GenException.h"
#include "InputXML.h"


void DeployInst::init(xmlNodePtr cur)
{
    InstModel::init(cur);

    /// get deployments
    xmlNodeSetPtr nodes = XMLinput->get_xpath_elements(cur,"model/DeployInst/deploy");
    
    for (int i=0;i<nodes->nodeNr;i++) 
    {
	xmlNodePtr deploy = nodes->nodeTab[i];
	string fac_name = XMLinput->get_xpath_content(deploy,"type");
	
	Model* facility = LI->getFacilityByName(fac_name);

	if (NULL == facility)
	    throw GenException("Facility '" 
			       + fac_name 
			       + "' is not defined in this problem.");

	if (!((RegionModel*)region)->isAllowedFacility(facility))
	    throw GenException("Facility '" 
			       + fac_name 
			       + "' is not an allowed facility for region '" 
			       + region->getName() +"'.");

	Model* new_facility = Model::create(facility);
	
	((FacilityModel*)new_facility)->setFacName(XMLinput->get_xpath_content(deploy,"name"));

	int start_month = atoi(XMLinput->get_xpath_content(deploy,"start"));

	if (start_month < 0)
	    throw GenException("You can't deploy a facility in the past.");

	deployment_map[start_month] = new_facility;
    }

}

void DeployInst::copy(DeployInst* src)
{
    InstModel::copy(src);

    deployment_map = src->deployment_map;

}

void DeployInst::print() 
{ 
    InstModel::print();

    cout << " with deployment schedule: " << endl;
    for (map<int,Model*>::iterator deploy=deployment_map.begin();
	 deploy!=deployment_map.end();
	 deploy++)
	cout << "\t\t\tFacility " << ((FacilityModel*)(*deploy).second)->getFacName()
	     << " ("  << (*deploy).second->getName() 
	     << ") is deployed in month " << (*deploy).first << endl;
};






