// InstModel.cpp
// Implements the InstModel class

#include "InstModel.h"

#include "GenException.h"
#include "Logician.h"
#include "InputXML.h"

/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

// Initialize the InstModel nextID to zero.
int InstModel::nextID = 0;

InstModel::InstModel(xmlNodePtr cur)
{
    /** 
     *  Generic initialization for Models
     */

    ID = nextID++;

    name = XMLinput->get_xpath_content(cur,"name");

    /** 
     * Generic initialization for Communicators
     */
    commType = MarketComm;

    /** 
     *  Specific initialization for InstModels
     */

   /// determine the parent from the XML input
    string region_name = XMLinput->get_xpath_content(cur,"../name");
    region = LI->getRegionByName(region_name);
    
    ((RegionModel*)region)->addInstitution(this);
	

}
