// StubFacility.cpp
// Implements the StubFacility class

#include "StubFacility.h"

#include "Logger.h"
#include "CycException.h"
#include "InputXML.h"

using namespace std;

/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
StubFacility::StubFacility() {};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
StubFacility::~StubFacility() {};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void StubFacility::init(xmlNodePtr cur) {
  FacilityModel::init(cur);
  /// move XML pointer to current model
  cur = XMLinput->get_xpath_element(cur,"model/StubFacility");
  /// initialize any StubFacility-specific datamembers here
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void StubFacility::copy(StubFacility* src) {
  FacilityModel::copy(src);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void StubFacility::copyFreshModel(Model* src) {
  copy(dynamic_cast<StubFacility*>(src));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void StubFacility::print() {
  FacilityModel::print();
};

/* ------------------- */ 


/* --------------------
 * all COMMUNICATOR classes have these members
 * --------------------
 */

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void StubFacility::receiveMessage(msg_ptr msg) {}

/* ------------------- */ 


/* --------------------
 * all FACILITYMODEL classes have these members
 * --------------------
 */

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
vector<rsrc_ptr> StubFacility::removeResource(msg_ptr order) {}
    
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void StubFacility::addResource(msg_ptr msg, std::vector<rsrc_ptr> manifest){}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void StubFacility::handleTick(int time){}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void StubFacility::handleTock(int time){}

/* ------------------- */ 


/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

extern "C" Model* constructStubFacility() {
  return new StubFacility();
}

/* ------------------- */ 

