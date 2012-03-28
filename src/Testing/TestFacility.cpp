// TestFacility.cpp
// Implements the TestFacility class

#include "TestFacility.h"
#include "Model.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
extern "C" Model* constructTestFacility() {
  return new TestFacility();
}

