#include "TestFacility.h"

extern "C" Model* constructTestFacility() {
  return new TestFacility();
}
