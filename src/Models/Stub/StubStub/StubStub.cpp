// StubStub.cpp
// Implements the StubStub class
#include <iostream>
#include "Logger.h"

#include "StubStub.h"

/* --------------------
 * all MODEL classes have these members
 * --------------------
 */
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
StubStub::StubStub() {};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
StubStub::~StubStub() {};
    
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void StubStub::init(xmlNodePtr cur) { 
  StubModel::init(cur); 
};
  
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void StubStub::copy(StubStub* src) { 
  StubModel::copy(src); 
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void StubStub::print() { 
  StubModel::print(); 
};




/* --------------------
 *  * all MODEL classes have these members
 *   * --------------------
 *    */

extern "C" Model* constructStubStub() {
      return new StubStub();
}

extern "C" void destructStubStub(Model* p) {
      delete p;
}

/* -------------------- */
