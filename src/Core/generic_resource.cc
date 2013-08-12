// Resource.cpp
// Implements the Resource Class

#include "generic_resource.h"

#include "error.h"
#include "logger.h"

namespace cyclus {

const ResourceType GenericResource::kType = "GenericResource";

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
GenericResource::Ptr GenericResource::Create(double quantity,
                                             std::string quality,
                                             std::string units) {
  GenericResource::Ptr r(new GenericResource(quantity, quality, units));
  r->tracker_.Create();
  return r;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
GenericResource::Ptr GenericResource::CreateUntracked(double quantity,
                                                      std::string quality,
                                                      std::string units) {
  GenericResource::Ptr r(new GenericResource(quantity, quality, units));
  r->tracker_.DontTrack();
  return r;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Resource::Ptr GenericResource::Clone() const {
  GenericResource* g = new GenericResource(*this);
  Resource::Ptr c = Resource::Ptr(g);
  g->tracker_.DontTrack();
  return c;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void GenericResource::Absorb(GenericResource::Ptr other) {
  if (other->units() != units()) {
    throw ValueError("incompatible resource types.");
  }
  quantity_ += other->quantity();
  other->quantity_ = 0;

  tracker_.Absorb(&other->tracker_);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
GenericResource::Ptr GenericResource::Extract(double quantity) {
  if (quantity > quantity_) {
    throw ValueError("Attempted to extract more quantity than exists.");
  }

  quantity_ -= quantity;

  GenericResource::Ptr other(new GenericResource(quantity, quality_, units_));
  tracker_.Extract(&other->tracker_);
  return other;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Resource::Ptr GenericResource::ExtractRes(double qty) {
  return boost::static_pointer_cast<Resource>(Extract(qty));
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
GenericResource::GenericResource(double quantity, std::string quality,
                                 std::string units)
  : units_(units), quality_(quality), quantity_(quantity), tracker_(this) { }

} // namespace cyclus
