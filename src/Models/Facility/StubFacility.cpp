// StubFacility.cpp
// Implements the StubFacility class
#include <iostream>

#include "StubFacility.h"

#include "Logician.h"
#include "GenException.h"
#include "InputXML.h"

/* --------------------
 * all MODEL classes have these members
 * --------------------
 */
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
StubFacility::StubFacility() {};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
StubFacility::~StubFacility() {};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void StubFacility::init(xmlNodePtr cur)
{
    FacilityModel::init(cur);

    /// move XML pointer to current model
    cur = XMLinput->get_xpath_element(cur,"model/StubFacility");
    /// initialize any StubFacility-specific datamembers here

}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void StubFacility::copy(StubFacility* src)
{
    FacilityModel::copy(src);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void StubFacility::copyFreshModel(Model* src)
{
  copy(dynamic_cast<StubFacility*>(src));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void StubFacility::print() 
{ 
    FacilityModel::print();
    
};


/* --------------------
 * all COMMUNICATOR classes have these members
 * --------------------
 */
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void StubFacility::sendMessage() {};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void StubFacility::receiveMessage(Message* msg) {};


/* --------------------
 * all FACILITYMODEL classes have these members
 * --------------------
 */

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void StubFacility::sendMaterial(Transaction trans, const Communicator* receiver){
};
    
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void StubFacility::receiveMaterial(Transaction trans, vector<Material*> manifest){
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void StubFacility::handleTick(int time){
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void handleTock(int time){
};


