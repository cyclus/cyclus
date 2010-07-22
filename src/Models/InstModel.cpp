// InstModel.cpp
// Implements the InstModel class

#include "InstModel.h"

/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

// Initialize the InstModel nextID to zero.
int InstModel::nextID = 0;

#include "Logician.h"
#include "InputXML.h"

void InstModel::init(xmlNodePtr cur)
{
    Model::init(cur);

    /** 
     *  Specific initialization for InstModels
     */
    
    /// determine the parent from the XML input
    string region_name = XMLinput->get_xpath_content(cur,"../name");
    region = LI->getRegionByName(region_name);
    
    ((RegionModel*)region)->addInstitution(this);

}

void InstModel::copy(InstModel* src)
{
    Model::copy(src);
    Communicator::copy(src);

    /** 
     *  Specific initialization for InstModels
     */
    
    region = src->region;
    ((RegionModel*)region)->addInstitution(this);
    
}

void InstModel::print()
{
    Model::print();

    cout << "in region " << region->getName();
}
