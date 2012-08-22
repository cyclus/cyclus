#include "RegionModel.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void TestRegion::receiveMessage(msg_ptr msg) {
  msg->setDir(DOWN_MSG);
}

void TestRegion::copyFreshModel(Model* model) { }

extern "C" Model* constructTestRegion() {
  return new TestRegion();
}
