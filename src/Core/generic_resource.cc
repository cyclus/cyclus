// generic_resource.cc

#include "generic_resource.h"

#include "error.h"
#include "logger.h"

namespace cyclus {

const ResourceType GenericResource::kType = "GenericResource";

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
GenericResource::Ptr GenericResource::Create(double quantity,
                                             std::string quality,
                                             std::string units,
                                             Context* ctx) {
  GenericResource::Ptr r(new GenericResource(quantity, quality, units, ctx));
  r->tracker_.Create();
  return r;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
GenericResource::Ptr GenericResource::CreateUntracked(double quantity,
                                                      std::string quality,
                                                      std::string units) {
  GenericResource::Ptr r(new GenericResource(quantity, quality, units, NULL));
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
  if (other->units() != units() || other->quality() != quality()) {
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

  GenericResource::Ptr other(new GenericResource(quantity, quality_, units_, ctx_));
  tracker_.Extract(&other->tracker_);
  return other;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Resource::Ptr GenericResource::ExtractRes(double qty) {
  return boost::static_pointer_cast<Resource>(Extract(qty));
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
GenericResource::GenericResource(double quantity, std::string quality,
                                 std::string units, Context* ctx)
  : units_(units), quality_(quality), quantity_(quantity), tracker_(this, ctx) { }

} // namespace cyclus
