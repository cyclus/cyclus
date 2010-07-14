// StubFacility.cpp
// Implements the StubFacility class
#include <iostream>

#include "StubFacility.h"

#include "Logician.h"
#include "GenException.h"
#include "InputXML.h"

void StubFacility::init(xmlNodePtr cur)
{
    FacilityModel::init(cur);

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

void StubFacility::copy(StubFacility* src)
{
    FacilityModel::copy(src);

    in_commods = src->in_commods;
    out_commods = src->out_commods;	
}

void StubFacility::print() 
{ 
    FacilityModel::print();

    cout << "converts commodities ";
    for (vector<Commodity*>::iterator commod=in_commods.begin();
 	 commod != in_commods.end();
	 commod++)
    {
	cout << (commod == in_commods.begin() ? "{" : ", " );
	cout << (*commod)->getName();
    }
    cout << "} into commodities ";
    for (vector<Commodity*>::iterator commod=out_commods.begin();
	 commod != out_commods.end();
	 commod++)
    {
	cout << (commod == out_commods.begin() ? "{" : ", " );
	cout << (*commod)->getName();
    }
    cout << "}" << endl;
    
};






