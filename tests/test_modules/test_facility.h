#ifndef CYCLUS_TESTS_TEST_MODULES_TEST_FACILITY_H_
#define CYCLUS_TESTS_TEST_MODULES_TEST_FACILITY_H_

#include "cyclus.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/// This is the simplest possible Facility, for testing
class TestFacility: public cyclus::Facility {
 public:
  TestFacility(cyclus::Context* ctx) : cyclus::Facility(ctx) {};
  virtual ~TestFacility() {};
  //#pragma cyclus
  
  virtual cyclus::Agent* Clone() { return new TestFacility(context()); };
  virtual void InitInv(cyclus::Inventories& inv) {};
  virtual cyclus::Inventories SnapshotInv() {return cyclus::Inventories();};

  void Tick(int time) {};
  void Tock(int time) {};
};

#endif  // CYCLUS_TESTS_TEST_MODULES_TEST_FACILITY_H_
