#if !defined _TESTINST_H_
#define _TESTINST_H_


#include "RegionModel.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/// This is the simplest possible Region, for testing
class TestRegion: public cyclus::RegionModel {
  
  void ReceiveMessage(cyclus::msg_ptr msg) {
    msg->SetDir(cyclus::DOWN_MSG);
  }
};

#endif
