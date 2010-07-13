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
     * Generic initialization for Communicators
     */
    commType = MarketComm;

}


/* --------------------
 * all FACILITYMODEL classes have these members
 * --------------------
 */


