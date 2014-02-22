#include "null_region.h"

namespace cyclus {

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
NullRegion::NullRegion(cyclus::Context* ctx)
    : cyclus::RegionModel(ctx) {};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
NullRegion::~NullRegion() {}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
extern "C" cyclus::Model* ConstructNullRegion(cyclus::Context* ctx) {
  return new NullRegion(ctx);
}

}  // namespce cyclus
