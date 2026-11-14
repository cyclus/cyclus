#include "unit_cost_test_facility.h"

extern "C" cyclus::Agent* ConstructUnitCostTestFacility(cyclus::Context* ctx) {
  return new cyclus::toolkit::UnitCostTestFacility(ctx);
}
