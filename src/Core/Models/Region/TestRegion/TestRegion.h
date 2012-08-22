#if !defined _TESTREGION_H_
#define _TESTREGION_H_

#include "RegionModel.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/// This is the simplest possible Region, for testing
class TestRegion: public RegionModel {
  
  void receiveMessage(msg_ptr msg);

  void copyFreshModel(Model* model);
};

#endif
