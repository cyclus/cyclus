// NullFacility.cpp
// Implements the NullFacility class
#include <iostream>

#include "NullFacility.h"

#include "Logician.h"
#include "GenException.h"
#include "InputXML.h"

NullFacility::NullFacility(xmlNodePtr cur)
    : FacilityModel(cur) // assign ID & increment
{

    in_commod = out_commod = NULL;

    /// move XML pointer to current model
    cur = XMLinput->get_xpath_element(cur,"model/NullFacility");

    /// all facilities require commodities - possibly many
    string commod_name;
    Commodity* new_commod;

    commod_name = XMLinput->get_xpath_content(cur,"incommodity");
    in_commod = LI->getCommodity(commod_name);
    if (NULL == in_commod)
	throw GenException("Input commodity '" + commod_name 
			   + "' does not exist for facility '" + getName() 
			   + "'.");

    commod_name = XMLinput->get_xpath_content(cur,"outcommodity");
    out_commod = LI->getCommodity(commod_name);
    if (NULL == out_commod)
	throw GenException("Input commodity '" + commod_name 
			   + "' does not exist for facility '" + getName() 
			   + "'.");
    
}


void NullFacility::print() 
{ 
    cout << "This is a NullFacility model with name " << name 
	 << " and ID: " << ID << " that converts commodity {"
	 << in_commod->getName()
	 << "} into commodity {"
	 << out_commod->getName()
	 << "}" << endl;
    
};



void NullFacility::receiveOfferRequest(OfferRequest* msg)
{
}

void NullFacility::sendMaterial(Transaction trans, Communicator* receiver)
{
}
    
void NullFacility::receiveMaterial(Transaction trans, vector<Material*> manifest)
{
}




