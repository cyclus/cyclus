// FacilityModel.cpp
// Implements the FacilityModel class

#include "FacilityModel.h"
#include "InputXML.h"
#include "GenException.h"
#include "Logician.h"

/* --------------------
 * all MODEL classes have these public members
 * --------------------
 */

// Initialize the FacilityModel nextID to zero.
int FacilityModel::nextID = 0;

FacilityModel::FacilityModel(xmlNodePtr cur)
{
    ID = nextID++;

    name = XMLinput->get_child_content(cur,"name");

    /// all facilities require commodities - possibly many
    string commod_name;
    Commodity* new_commod;
    xmlNodeSetPtr nodes = XMLinput->get_elements(cur,"incommodity");

    for (int i=0;i<nodes->nodeNr;i++)
    {
	commod_name = (const char*)nodes->nodeTab[i]->children->content;
	new_commod = LI->getCommodity(commod_name);
	if (NULL == new_commod)
	    throw GenException("That input commodity doesn't exist.");
	in_commods.push_back(new_commod);
    }

    nodes = XMLinput->get_elements(cur,"outcommodity");

    for (int i=0;i<nodes->nodeNr;i++)
    {
	commod_name = (const char*)nodes->nodeTab[i]->children->content;
	new_commod = LI->getCommodity(commod_name);
	if (NULL == new_commod)
	    throw GenException("That output commodity doesn't exist.");
	out_commods.push_back(new_commod);
    }
	
}


/* --------------------
 * all FACILITYMODEL classes have these protected members
 * --------------------
 */
