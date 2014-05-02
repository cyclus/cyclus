#ifndef CYCLUS_TESTS_TEST_MODULES_TEST_FACILITY_H_
#define CYCLUS_TESTS_TEST_MODULES_TEST_FACILITY_H_

#include "cyclus.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/// This is the simplest possible Facility, for testing
class TestFacility: public cyclus::Facility {
 public:
  static std::string proto_name() { return "test_fac_prototype"; }
  static std::string agent_impl() { return "test_fac_impl"; }
                                  
  TestFacility(cyclus::Context* ctx) : cyclus::Facility(ctx) {
    cyclus::Agent::prototype(proto_name());
    cyclus::Agent::agent_impl(agent_impl());
  }
  virtual ~TestFacility() {};
  
  virtual cyclus::Agent* Clone() { return new TestFacility(context()); };
  virtual void InitInv(cyclus::Inventories& inv) {};
  virtual cyclus::Inventories SnapshotInv() {return cyclus::Inventories();};

  void Tick(int time) {};
  void Tock(int time) {};
};

#endif  // CYCLUS_TESTS_TEST_MODULES_TEST_FACILITY_H_
