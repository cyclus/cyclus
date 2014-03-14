// generic_resource.cc

#include "generic_resource.h"

#include "error.h"
#include "logger.h"

namespace cyclus {

const ResourceType GenericResource::kType = "GenericResource";

std::map<std::string, int> GenericResource::stateids_;
int GenericResource::next_state_ = 1;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
GenericResource::Ptr GenericResource::Create(Model* creator,
                                             double quantity,
                                             std::string quality) {
  GenericResource::Ptr r(new GenericResource(creator->context(), quantity,
                                             quality));
  r->tracker_.Create(creator);

  if (stateids_.count(quality) == 0) {
    stateids_[quality] = next_state_++;
    creator->context()->NewDatum("GenericResources")
    ->AddVal("StateId", stateids_[quality])
    ->AddVal("Quality", quality)
    ->Record();
  }
  return r;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
GenericResource::Ptr GenericResource::CreateUntracked(double quantity,
                                                      std::string quality) {
  GenericResource::Ptr r(new GenericResource(NULL, quantity, quality));
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
  if (other->quality() != quality()) {
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

  GenericResource::Ptr other(new GenericResource(ctx_, quantity, quality_));
  tracker_.Extract(&other->tracker_);
  return other;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Resource::Ptr GenericResource::ExtractRes(double qty) {
  return boost::static_pointer_cast<Resource>(Extract(qty));
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
GenericResource::GenericResource(Context* ctx, double quantity,
                                 std::string quality)
  : quality_(quality), quantity_(quantity), tracker_(ctx, this),
    ctx_(ctx) {}

} // namespace cyclus

