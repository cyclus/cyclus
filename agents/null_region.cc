#include "null_region.h"

namespace cyclus {

NullRegion::NullRegion(cyclus::Context* ctx) : cyclus::Region(ctx) {}

NullRegion::~NullRegion() {}

void NullRegion::EnterNotify() {
  InitializeCosts();
}

extern "C" cyclus::Agent* ConstructNullRegion(cyclus::Context* ctx) {
  return new NullRegion(ctx);
}

}  // namespace cyclus
