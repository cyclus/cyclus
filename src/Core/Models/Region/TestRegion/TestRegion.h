#if !defined _TESTINST_H_
#define _TESTINST_H_


#include "RegionModel.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/// This is the simplest possible Region, for testing
class TestRegion: public cyclus::RegionModel {
  
  void receiveMessage(cyclus::msg_ptr msg) {
    msg->setDir(cyclus::DOWN_MSG);
  }
};

#endif
