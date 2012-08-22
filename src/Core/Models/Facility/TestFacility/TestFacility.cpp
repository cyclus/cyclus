
#include "FacilityModel.h"
#include "Material.h"
#include "TestFacility.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void TestFacility::receiveMessage(msg_ptr msg) {
  msg->setDir(DOWN_MSG);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void TestFacility::copyFreshModel(Model* src) { 
  copy(dynamic_cast<TestFacility*>(src)); 
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void TestFacility::receiveMaterial(Transaction trans, 
    std::vector<mat_rsrc_ptr> manifest) { 
}

extern "C" Model* constructTestFacility() {
  return new TestFacility();
}
