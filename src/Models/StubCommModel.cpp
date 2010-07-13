// StubCommModel.cpp
// Implements the StubCommModel class

#include "StubCommModel.h"


#include "GenException.h"
#include "Logician.h"
#include "InputXML.h"

/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

// Initialize the StubCommModel nextID to zero.
int StubCommModel::nextID = 0;

StubCommModel::StubCommModel(xmlNodePtr cur)
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
     *  Specific initialization for StubCommModels
     */

}
