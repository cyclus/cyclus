// DeployInst.cpp
// Implements the DeployInst class
#include <iostream>

#include "DeployInst.h"

#include "Logician.h"
#include "GenException.h"
#include "InputXML.h"


DeployInst::DeployInst(xmlNodePtr cur)
    : InstModel(cur) // assign & incrememt ID
{
    /// get deployments
    xmlNodeSetPtr nodes = XMLinput->get_xpath_elements(cur,"model/DeployInst/deploy");
    
    for (int i=0;i<nodes->nodeNr;i++) 
    {
	xmlNodePtr deploy = nodes->nodeTab[i];
	string fac_name = XMLinput->get_xpath_content(deploy,"name");
	
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

	int start_month = atoi(XMLinput->get_xpath_content(deploy,"start"));

	if (start_month < 0)
	    throw GenException("You can't deploy a facility in the past.");

	deployment_map[start_month] = facility;
    }

}

void DeployInst::print() 
{ 
    cout << "\tThis is a DeployInst model with name " << name 
	 << " and ID: " << ID << endl;

    for (map<int,Model*>::iterator deploy=deployment_map.begin();
	 deploy!=deployment_map.end();
	 deploy++)
	cout << "\t\tFacility type: " << (*deploy).second->getName() 
	     << " is deployed in month " << (*deploy).first << endl;


};






