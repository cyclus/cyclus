// resource.cc
// Implements the Resource Class

#include "generic_resource.h"

#include "error.h"
#include "logger.h"
#include "event_manager.h"

namespace cyclus {

bool GenericResource::type_is_recorded_ = false;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
GenericResource::GenericResource(std::string units,
                                 std::string quality, double quantity) : Resource() {
  units_ = units;
  quality_ = quality;
  quantity_ = quantity;
  recorded_ = false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int GenericResource::StateID() {
  return 0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
GenericResource::GenericResource(const GenericResource& other) {
  units_ = other.units_;
  quality_ = other.quality_;
  quantity_ = other.quantity_;
  recorded_ = false;
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Resource::Ptr GenericResource::clone() {
  CLOG(LEV_DEBUG2) << "GenericResource ID=" << ID_ << " was cloned.";
  Print();
  return Resource::Ptr(new GenericResource(*this));
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void GenericResource::Print() {
  CLOG(LEV_DEBUG3) << "GenericResource ID=" << ID_ << ", quality=" << quality_
                   << ", quantity=" << quantity_ << ", units=" << units_;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool GenericResource::CheckQuality(Resource::Ptr other) {
  CLOG(LEV_DEBUG1) << "GenericResource is checking quality, this = "
                   << units_ << " other = " << other->units();

  return units_ == other->units();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void GenericResource::Absorb(GenericResource::Ptr other) {
  if (! CheckQuality(boost::dynamic_pointer_cast<Resource>(other))) {
    throw ValueError("incompatible resource types.");
  }

  quantity_ += other->quantity();
  other->SetQuantity(0);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
GenericResource::Ptr GenericResource::Extract(double quantity) {
  if (quantity > quantity_) {
    throw ValueError("Attempted to extract more quantity than exists.");
  }

  quantity_ -= quantity;

  return GenericResource::Ptr(new GenericResource(units_, quality_, quantity));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void GenericResource::AddToTable() {
  Resource::AddToTable();

  if (recorded_) {
    return;
  }
  recorded_ = true;

  EM->NewEvent("GenericResources")
  ->AddVal("ResourceID", ID())
  ->AddVal("Quality", quality())
  ->Record();
}
} // namespace cyclus
