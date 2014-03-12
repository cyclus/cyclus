#ifndef CYCLUS_TESTS_TEST_MODULES_TEST_FACILITY_H_
#define CYCLUS_TESTS_TEST_MODULES_TEST_FACILITY_H_

#include "cyclus.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/// This is the simplest possible Facility, for testing
class TestFacility: public cyclus::FacilityModel {
 public:
  TestFacility(cyclus::Context* ctx) : cyclus::FacilityModel(ctx) {};
      
  virtual cyclus::Model* Clone() { return new TestFacility(context()); };

  virtual void InitInv(cyclus::Inventories& inv) {};
  virtual cyclus::Inventories SnapshotInv() {return cyclus::Inventories();};

  void CloneModuleMembersFrom(cyclus::FacilityModel* source) {};
  void Tick(int time) {};
  void Tock(int time) {};
};

#endif  // CYCLUS_TESTS_TEST_MODULES_TEST_FACILITY_H_
