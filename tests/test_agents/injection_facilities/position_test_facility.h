#ifndef CYCLUS_TESTS_TEST_AGENTS_INJECTION_FACILITIES_POSITION_TEST_FACILITY_H_
#define CYCLUS_TESTS_TEST_AGENTS_INJECTION_FACILITIES_POSITION_TEST_FACILITY_H_

#include "cyclus.h"

namespace cyclus {
namespace toolkit {

/// A deliberately small host for position.cycpp.h tests.
class PositionTestFacility : public cyclus::Facility {
 public:
  explicit PositionTestFacility(cyclus::Context* ctx) : cyclus::Facility(ctx) {
    spec("tests:InjectionTestAgents:PositionTestFacility");
  }

  virtual ~PositionTestFacility() {}

  // Generate the Cycpp state, schema, clone, and persistence methods.
#pragma cyclus

  virtual void EnterNotify() {
    cyclus::Facility::EnterNotify();
    InitializePosition();
  }

  virtual void Tick() {}
  virtual void Tock() {}

  // Keep the injectable public so its intentionally provided API can be
  // exercised directly. Production archetypes may include it privately.
#include "toolkit/position.cycpp.h"
};

}  // namespace toolkit
}  // namespace cyclus

#endif  // CYCLUS_TESTS_TEST_AGENTS_INJECTION_FACILITIES_POSITION_TEST_FACILITY_H_
