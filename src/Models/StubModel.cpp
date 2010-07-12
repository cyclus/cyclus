// StubModel.cpp
// Implements the StubModel class

#include "StubModel.h"


#include "GenException.h"
#include "Logician.h"
#include "InputXML.h"

/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

// Initialize the StubModel nextID to zero.
int StubModel::nextID = 0;

StubModel::StubModel(xmlNodePtr cur)
{
    /** 
     *  Generic initialization for Models
     */

    ID = nextID++;

    name = XMLinput->get_xpath_content(cur,"name");

    /** 
     *  Specific initialization for StubModels
     */

}
