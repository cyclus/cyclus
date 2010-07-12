// FacilityModel.cpp
// Implements the FacilityModel class

#include "FacilityModel.h"
#include "InputXML.h"
#include "GenException.h"
#include "Logician.h"

/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

// Initialize the FacilityModel nextID to zero.
int FacilityModel::nextID = 0;

FacilityModel::FacilityModel(xmlNodePtr cur)
{
    /** 
     *  Generic initialization for Models
     */

    ID = nextID++;

    name = XMLinput->get_xpath_content(cur,"name");

    /** 
     *  Specific initialization for FacilityModels
     */

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


/* --------------------
 * all FACILITYMODEL classes have these members
 * --------------------
 */


