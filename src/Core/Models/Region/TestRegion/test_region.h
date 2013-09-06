#if !defined TEST_REGION_H_
#define TEST_REGION_H_

#include "region_model.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/// This is the simplest possible Region, for testing
class TestRegion: public cyclus::RegionModel {
 public:
  TestRegion(cyclus::Context* ctx) : cyclus::RegionModel(ctx) { };

  void ReceiveMessage(cyclus::Message::Ptr msg) {
    msg->SetDir(cyclus::DOWN_MSG);
  }
};

#endif
