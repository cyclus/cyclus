// RegionModel.cpp
// Implements the RegionModel class

#include "RegionModel.h"

#include "GenException.h"
#include "Logician.h"
#include "InputXML.h"

/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

// Initialize the RegionModel nextID to zero.
int RegionModel::nextID = 0;

RegionModel::RegionModel(xmlNodePtr cur)
{
    /** 
     *  Generic initialization for Models
     */

    ID = nextID++;

    name = XMLinput->get_xpath_content(cur,"name");

    /** 
     *  Specific initialization for RegionModels
     */

    /// all regions require allowed facilities - possibly many
    xmlNodeSetPtr nodes = XMLinput->get_xpath_elements(cur,"allowedfacility");

    string fac_name;
    Model* new_fac;

    for (int i=0;i<nodes->nodeNr;i++)
    {
	fac_name = (const char*)nodes->nodeTab[i]->children->content;
	new_fac = LI->getFacilityByName(fac_name);
	if (NULL == new_fac)
	    throw GenException("Facility " + fac_name + " is not defined in this simulation.");
	allowedFacilities.insert(new_fac);
    }
    
}

/* --------------------
 * all REGIONMODEL classes have these members
 * --------------------
 */
