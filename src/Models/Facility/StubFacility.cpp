// StubFacility.cpp
// Implements the StubFacility class
#include <iostream>

#include "StubFacility.h"

#include "Logician.h"
#include "GenException.h"
#include "InputXML.h"

StubFacility::StubFacility(xmlNodePtr cur)
    : FacilityModel(cur) // assign ID & increment
{

    /** 
     *  Allow a Stub Facility to have many input/output commodities
     */

    /// move XML pointer to current model
    cur = XMLinput->get_xpath_element(cur,"model/StubFacility");

    /// all facilities require commodities - possibly many
    string commod_name;
    Commodity* new_commod;
    xmlNodeSetPtr nodes = XMLinput->get_xpath_elements(cur,"incommodity");

    for (int i=0;i<nodes->nodeNr;i++)
    {
	commod_name = (const char*)nodes->nodeTab[i]->children->content;
	new_commod = LI->getCommodity(commod_name);
	if (NULL == new_commod)
	    throw GenException("Input commodity '" + commod_name 
			       + "' does not exist for facility '" + getName() 
			       + "'.");
	in_commods.push_back(new_commod);
    }

    nodes = XMLinput->get_xpath_elements(cur,"outcommodity");

    for (int i=0;i<nodes->nodeNr;i++)
    {
	commod_name = (const char*)nodes->nodeTab[i]->children->content;
	new_commod = LI->getCommodity(commod_name);
	if (NULL == new_commod)
	    throw GenException("Output commodity '" + commod_name 
			       + "' does not exist for facility '" + getName() 
			       + "'.");
	out_commods.push_back(new_commod);
    }
	

}


void StubFacility::print() 
{ 
    cout << "This is a StubFacility model with name " << name 
	 << " and ID: " << ID << " that converts commodities {";
    for (vector<Commodity*>::iterator commod=in_commods.begin();
 	 commod != in_commods.end();)
    {
	cout << (*commod)->getName();
	cout << (++commod != in_commods.end() ? "," : "" );
    }
    cout << "} into commodities {";
    for (vector<Commodity*>::iterator commod=out_commods.begin();
	 commod != out_commods.end();)
    {
	cout << (*commod)->getName();
	cout << (++commod != out_commods.end() ? "," : "" );
    }
    cout << "}" << endl;
    
};






