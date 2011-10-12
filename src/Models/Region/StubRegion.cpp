// StubRegion.cpp
// Implements the StubRegion class
#include <iostream>

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
void StubRegion::receiveMessage(Message* msg) {};


/* --------------------
 * all REGIONMODEL classes have these members
 * --------------------
 */








