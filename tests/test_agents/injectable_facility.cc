#include "injectable_facility.h"

// Factory for the standalone module; implementations live in the header.
extern "C" cyclus::Agent* ConstructInjectableFacility(cyclus::Context* ctx) {
  return new cyclus::toolkit::InjectableFacility(ctx);
}
