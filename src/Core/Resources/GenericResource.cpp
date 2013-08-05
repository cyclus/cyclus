// Resource.cpp
// Implements the Resource Class

#include "GenericResource.h"

#include "error.h"
#include "Logger.h"
#include "EventManager.h"

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
int GenericResource::stateID() {
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
rsrc_ptr GenericResource::clone() {
  CLOG(LEV_DEBUG2) << "GenericResource ID=" << ID_ << " was cloned.";
  print();
  return rsrc_ptr(new GenericResource(*this));
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
void GenericResource::print() {
  CLOG(LEV_DEBUG3) << "GenericResource ID=" << ID_ << ", quality=" << quality_
                   << ", quantity=" << quantity_ << ", units=" << units_;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
bool GenericResource::checkQuality(rsrc_ptr other)
{
  CLOG(LEV_DEBUG1) << "GenericResource is checking quality, this = " 
                   << units_ << " other = " << other->units();
  
  return units_ == other->units();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void GenericResource::absorb(gen_rsrc_ptr other) {
  if (! checkQuality(boost::dynamic_pointer_cast<Resource>(other))) {
    throw ValueError("incompatible resource types.");
  }

  quantity_ += other->quantity();
  other->setQuantity(0);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
gen_rsrc_ptr GenericResource::extract(double quantity) {
  if (quantity > quantity_) {
    throw ValueError("Attempted to extract more quantity than exists.");
  }

  quantity_ -= quantity;

  return gen_rsrc_ptr(new GenericResource(units_, quality_, quantity));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void GenericResource::addToTable() {
  Resource::addToTable();

  if (recorded_) {
    return;
  }
  recorded_ = true;

  EM->newEvent("GenericResources")
    ->addVal("ResourceID", ID())
    ->addVal("Quality", quality())
    ->record();
}
} // namespace cyclus
