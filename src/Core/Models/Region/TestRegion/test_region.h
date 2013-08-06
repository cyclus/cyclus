#if !defined TEST_REGION_H_
#define TEST_REGION_H_

#include "region_model.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/// This is the simplest possible Region, for testing
class TestRegion: public cyclus::RegionModel {
  
  void ReceiveMessage(cyclus::msg_ptr msg) {
    msg->SetDir(cyclus::DOWN_MSG);
  }
};

#endif
