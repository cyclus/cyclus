#ifndef CYCLUS_TESTS_TOOLKIT_INJECTABLE_FACILITY_H_
#define CYCLUS_TESTS_TOOLKIT_INJECTABLE_FACILITY_H_

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
  explicit InjectableFacility(cyclus::Context* ctx);
  virtual ~InjectableFacility() {}

#pragma cyclus decl

  virtual void EnterNotify();
  virtual void Tick();
  virtual void Tock();

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

#endif  // CYCLUS_TESTS_TOOLKIT_INJECTABLE_FACILITY_H_
