#if !defined _TESTINST_H_
#define _TESTINST_H_


#include "RegionModel.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/// This is the simplest possible Region, for testing
class TestRegion: public RegionModel {
  
  void receiveMessage(msg_ptr msg) {
    msg->setDir(DOWN_MSG);
  }
};

#endif
