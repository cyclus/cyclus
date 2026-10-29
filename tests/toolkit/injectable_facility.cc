#include "injectable_facility.h"

namespace cyclus {
namespace toolkit {

InjectableFacility::InjectableFacility(cyclus::Context* ctx)
    : cyclus::Facility(ctx), tracker_({&input_}) {
  spec(":tests:InjectableFacility");
  input_.capacity(10);
  output_.capacity(10);
}

#pragma cyclus def clone cyclus::toolkit::InjectableFacility
#pragma cyclus def schema cyclus::toolkit::InjectableFacility
#pragma cyclus def annotations cyclus::toolkit::InjectableFacility
#pragma cyclus def infiletodb cyclus::toolkit::InjectableFacility
#pragma cyclus def snapshot cyclus::toolkit::InjectableFacility
#pragma cyclus def snapshotinv cyclus::toolkit::InjectableFacility
#pragma cyclus def initinv cyclus::toolkit::InjectableFacility
#pragma cyclus def initfromdb cyclus::toolkit::InjectableFacility
#pragma cyclus def initfromcopy cyclus::toolkit::InjectableFacility

void InjectableFacility::EnterNotify() {
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

void InjectableFacility::Tick() {
  if (input_.quantity() > 0) {
    output_.Push(input_.Pop(input_.quantity()));
  }
}

void InjectableFacility::Tock() {}

}  // namespace toolkit
}  // namespace cyclus
