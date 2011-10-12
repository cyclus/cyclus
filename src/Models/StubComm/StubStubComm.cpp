// StubStubCommComm.cpp
// Implements the StubStubCommComm class
#include <iostream>

#include "StubStubComm.h"

/* --------------------
 * all MODEL classes have these members
 * --------------------
 */
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
StubStubComm::StubStubComm() {};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
StubStubComm::~StubStubComm() {};
    
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void StubStubComm::init(xmlNodePtr cur) { 
  StubCommModel::init(cur); 
};
  
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void StubStubComm::copy(StubStubComm* src) { 
  StubCommModel::copy(src); 
};


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void StubStubComm::print() { 
  StubCommModel::print(); 
};

/* --------------------
 * all COMMUNICATOR classes have these members
 * --------------------
 */

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void StubStubComm::receiveMessage(Message* msg) {};


/* --------------------
 * all STUBCOMMMODEL classes have these members
 * --------------------
 */











