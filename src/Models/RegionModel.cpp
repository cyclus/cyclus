// RegionModel.cpp
// Implements the RegionModel class

#include "RegionModel.h"

#include "GenException.h"
#include "Logician.h"
#include "InputXML.h"

/* --------------------
 * all MODEL classes have these public members
 * --------------------
 */

// Initialize the RegionModel nextID to zero.
int RegionModel::nextID = 0;

RegionModel::RegionModel(xmlNodePtr cur)
{
    ID = nextID++;

    name = XMLinput->get_child_content(cur,"name");

    /// all regions require allowed facilities - possibly many
    string fac_name;
    Model* new_fac;
    xmlNodeSetPtr nodes = XMLinput->get_elements(cur,"allowedfacility");

    for (int i=0;i<nodes->nodeNr;i++)
    {
	fac_name = (const char*)nodes->nodeTab[i]->children->content;
	new_fac = LI->getFacilityByName(fac_name);
	if (NULL == new_fac)
	    throw GenException("That allowed facility doesn't exist.");
	allowedFacilities.push_back(new_fac);
    }

}

/* --------------------
 * all REGIONMODEL classes have these protected members
 * --------------------
 */
