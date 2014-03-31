#ifndef CYCLUS_TESTS_TEST_MODULES_TEST_REGION_H_
#define CYCLUS_TESTS_TEST_MODULES_TEST_REGION_H_

#include "cyclus.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/// This is the simplest possible Region, for testing
class TestRegion: public cyclus::RegionModel {
 public:
  TestRegion(cyclus::Context* ctx) : cyclus::RegionModel(ctx) {};
      
  virtual cyclus::Model* Clone() {return new TestRegion(context());};
};

#endif  // CYCLUS_TESTS_TEST_MODULES_TEST_REGION_H_
