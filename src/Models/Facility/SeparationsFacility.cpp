// SeparationsFacility.cpp
// Implements the SeparationsFacility class
#include <iostream>

#include "SeparationsFacility.h"

#include "Logician.h"
#include "GenException.h"
#include "InputXML.h"

/*
 * TICK
 * If there are ordersWaiting, prepare and send an appropriate 
 * request for raw material.
 * If there is capacity to produce any extra material next month
 * prepare and send an appropriate offer of SWUs.
 *
 * TOCK
 * Process as much raw stock material as capacity will allow.
 * Send appropriate materials to fill ordersWaiting.
 *
 * RECIEVE MATERIAL
 * put it in stocks
 *
 * SEND MATERIAL
 * pull it from inventory
 * decrement ordersWaiting
 */

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SeparationsFacility::SeparationsFacility() {};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SeparationsFacility::~SeparationsFacility() {};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void SeparationsFacility::init(xmlNodePtr cur)
{
    FacilityModel::init(cur);

    /// move XML pointer to current model
    cur = XMLinput->get_xpath_element(cur,"model/SeparationsFacility");
    /// initialize any SeparationsFacility-specific datamembers here

}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void SeparationsFacility::copy(SeparationsFacility* src)
{
    FacilityModel::copy(src);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void SeparationsFacility::print() 
{ 
    FacilityModel::print();
    
};


/* --------------------
 * all COMMUNICATOR classes have these members
 * --------------------
 */
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SeparationsFacility::sendMessage() {};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SeparationsFacility::receiveMessage(Message* msg) {};


/* --------------------
 * all FACILITYMODEL classes have these members
 * --------------------
 */

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SeparationsFacility::sendMaterial(Transaction trans, const Communicator* receiver){
};
    
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SeparationsFacility::receiveMaterial(Transaction trans, vector<Material*> manifest){
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SeparationsFacility::handleTick(int time){
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void handleTock(int time){
};


