// FixedInst.cpp
// Implements the FixedInst class
#include <iostream>

#include "FixedInst.h"

#include "FacilityModel.h"

#include "Logician.h"
#include "GenException.h"
#include "InputXML.h"


void FixedInst::init(xmlNodePtr cur)
{
    InstModel::init(cur);

    /// get facility list
    xmlNodeSetPtr nodes = XMLinput->get_xpath_elements(cur,"model/FixedInst/facility");
    
    for (int i=0;i<nodes->nodeNr;i++) 
    {
	xmlNodePtr fac_node = nodes->nodeTab[i];
	string fac_name = XMLinput->get_xpath_content(fac_node,"type");
	
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

	((FacilityModel*)new_facility)->setFacName(XMLinput->get_xpath_content(fac_node,"name"));
	facilities.push_back(new_facility);

	

    }


}

void FixedInst::copy(FixedInst* src)
{
    InstModel::copy(src);

    facilities = src->facilities;

}

void FixedInst::print() 
{ 
    InstModel::print();

    cout << " and the following permanent facilities: " << endl;
    for (vector<Model*>::iterator fac=facilities.begin(); fac != facilities.end(); fac++)
	cout << "\t\t* " << ((FacilityModel*)(*fac))->getFacName()
	     << " (" << (*fac)->getName() << ")" << endl;



};






