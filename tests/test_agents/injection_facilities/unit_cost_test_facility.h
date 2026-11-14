#ifndef CYCLUS_TESTS_TEST_AGENTS_INJECTION_FACILITIES_UNIT_COST_TEST_FACILITY_H_
#define CYCLUS_TESTS_TEST_AGENTS_INJECTION_FACILITIES_UNIT_COST_TEST_FACILITY_H_

#include "cyclus.h"

#pragma cyclus exec from cyclus.system import CY_LARGE_DOUBLE

namespace cyclus {
namespace toolkit {

/// A deliberately small host for unit_cost.cycpp.h tests.
class UnitCostTestFacility : public cyclus::Facility {
 public:
  explicit UnitCostTestFacility(cyclus::Context* ctx) : cyclus::Facility(ctx) {
    spec("tests:InjectionTestAgents:UnitCostTestFacility");
  }

  virtual ~UnitCostTestFacility() {}

  // Generate the Cycpp state, schema, clone, and persistence methods.
#pragma cyclus

  virtual void Tick() {}
  virtual void Tock() {}

  // Keep the injectable public so its intentionally provided API can be
  // exercised directly. Production archetypes may include it privately.
#include "toolkit/unit_cost.cycpp.h"
};

}  // namespace toolkit
}  // namespace cyclus

#endif  // CYCLUS_TESTS_TEST_AGENTS_INJECTION_FACILITIES_UNIT_COST_TEST_FACILITY_H_
