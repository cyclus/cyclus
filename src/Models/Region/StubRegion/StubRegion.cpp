// StubRegion.cpp
// Implements the StubRegion class
#include <iostream>
#include "Logger.h"

#include "StubRegion.h"





/* --------------------
 * all MODEL classes have these members
 * --------------------
 */
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
StubRegion::StubRegion() {};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
StubRegion::~StubRegion() {};
    
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void StubRegion::init(xmlNodePtr cur) { 
  RegionModel::init(cur); 
};
  
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void StubRegion::copy(StubRegion* src) { 
  RegionModel::copy(src); 
};


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void StubRegion::print() { 
  RegionModel::print(); 
};

/* --------------------
 * all COMMUNICATOR classes have these members
 * --------------------
 */

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void StubRegion::receiveMessage(msg_ptr msg) {};


/* --------------------
 * all REGIONMODEL classes have these members
 * --------------------
 */


/* --------------------
   output database info
 * --------------------
 */
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
std::string StubRegion::outputDir_ = "/stub";

/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

extern "C" Model* constructStubRegion() {
    return new StubRegion();
}


/* -------------------- */
