#include "position_test_facility.h"

extern "C" cyclus::Agent* ConstructPositionTestFacility(
    cyclus::Context* ctx) {
  return new cyclus::toolkit::PositionTestFacility(ctx);
}
