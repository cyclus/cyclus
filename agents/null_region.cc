#include "null_region.h"

namespace cyclus {

NullRegion::NullRegion(cyclus::Context* ctx) : cyclus::Region(ctx) {}

NullRegion::~NullRegion() {}

extern "C" cyclus::Agent* ConstructNullRegion(cyclus::Context* ctx) {
  return new NullRegion(ctx);
}

}  // namespce cyclus
