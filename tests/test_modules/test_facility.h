#ifndef CYCLUS_TESTS_TEST_MODULES_TEST_FACILITY_H_
#define CYCLUS_TESTS_TEST_MODULES_TEST_FACILITY_H_

#include "cyclus.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/// This is the simplest possible Facility, for testing
class TestFacility: public cyclus::Facility {
 public:
  static std::string proto_name() { return "test_fac_prototype"; }
  static std::string spec() { return "test_fac_impl"; }
                                  
  TestFacility(cyclus::Context* ctx) : cyclus::Facility(ctx) {
    cyclus::Agent::prototype(proto_name());
    cyclus::Agent::spec(spec());
  }
  virtual ~TestFacility() {};
  
  virtual cyclus::Agent* Clone() { return new TestFacility(context()); };
  virtual void InitInv(cyclus::Inventories& inv) {};
  virtual cyclus::Inventories SnapshotInv() {return cyclus::Inventories();};

  void Tick(int time) {};
  void Tock(int time) {};

  #pragma cyclus var dict(shape=[10])
  std::vector<double> sized_test;
};

#endif  // CYCLUS_TESTS_TEST_MODULES_TEST_FACILITY_H_
