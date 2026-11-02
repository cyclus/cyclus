#ifndef CYCLUS_TESTS_TEST_AGENTS_INJECTABLE_FACILITY_H_
#define CYCLUS_TESTS_TEST_AGENTS_INJECTABLE_FACILITY_H_

#include "cyclus.h"

#pragma cyclus exec from cyclus.system import CY_LARGE_DOUBLE, CY_LARGE_INT

namespace cyclus {
namespace toolkit {

/// A small host for testing code injected into an archetype by cycpp headers.
/// Add snippets below and their setup to EnterNotify, Tick, or Tock as needed.
/// Shared tests discover annotated scalar state automatically; add explicit
/// configuration and behavior checks when a snippet needs more than defaults.
class InjectableFacility : public cyclus::Facility {
  friend class InjectableFacilityTest;

 public:
  explicit InjectableFacility(cyclus::Context* ctx)
      : cyclus::Facility(ctx), tracker_({&input_}) {
    spec(":tests:InjectableFacility");
    input_.capacity(10);
    output_.capacity(10);
  }

  virtual ~InjectableFacility() {}

  // Generate state-handling methods inside the class, making them inline.
#pragma cyclus

  virtual void EnterNotify() {
    cyclus::Facility::EnterNotify();
    InitializePosition();
    InitBuyPolicyParameters();
    tracker_.set_capacity(10);
    buy_policy.Init(this, &input_, "input", &tracker_, 4,
                    active_dist_, dormant_dist_, size_dist_)
        .Set("injectable_in",
             cyclus::Composition::CreateFromAtom(cyclus::CompMap()),
             unit_cost_mod)
        .Start();
    sell_policy.Init(this, &output_, "output", 4, false, sell_quantity)
        .Set("injectable_out")
        .SetUnitCost(unit_cost)
        .Start();
  }

  virtual void Tick() {
    if (input_.quantity() > 0) {
      output_.Push(input_.Pop(input_.quantity()));
    }
  }

  virtual void Tock() {}

 private:
  // Keep snippets private, as recommended for production archetypes.
  #include "toolkit/position.cycpp.h"
  #include "toolkit/unit_cost.cycpp.h"
  #include "toolkit/matl_buy_policy.cycpp.h"
  #include "toolkit/matl_sell_policy.cycpp.h"

  // These buffers support lifecycle checks only. State round trips test
  // injected configuration, not material-inventory persistence.
  cyclus::toolkit::ResBuf<cyclus::Material> input_;
  cyclus::toolkit::ResBuf<cyclus::Material> output_;
  cyclus::toolkit::TotalInvTracker tracker_;
};

}  // namespace toolkit
}  // namespace cyclus

#endif  // CYCLUS_TESTS_TEST_AGENTS_INJECTABLE_FACILITY_H_
