#include "product.h"

#include "error.h"
#include "logger.h"

namespace cyclus {

const ResourceType Product::kType = "Product";

std::map<std::string, int> Product::qualids_;
int Product::next_qualid_ = 1;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Product::Ptr Product::Create(Agent* creator, double quantity,
                             std::string quality) {
  if (qualids_.count(quality) == 0) {
    qualids_[quality] = next_qualid_++;
    creator->context()->NewDatum("Products")
        ->AddVal("QualId", qualids_[quality])
        ->AddVal("Quality", quality)
        ->Record();
  }

  // the next lines must come after qual id setting
  Product::Ptr r(new Product(creator->context(), quantity, quality));
  r->tracker_.Create(creator);
  return r;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Product::Ptr Product::CreateUntracked(double quantity,
                                      std::string quality) {
  Product::Ptr r(new Product(NULL, quantity, quality));
  r->tracker_.DontTrack();
  return r;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Resource::Ptr Product::Clone() const {
  Product* g = new Product(*this);
  Resource::Ptr c = Resource::Ptr(g);
  g->tracker_.DontTrack();
  return c;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Product::Absorb(Product::Ptr other) {
  if (other->quality() != quality()) {
    throw ValueError("incompatible resource types.");
  }
  quantity_ += other->quantity();
  other->quantity_ = 0;

  tracker_.Absorb(&other->tracker_);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Product::Ptr Product::Extract(double quantity) {
  if (quantity > quantity_) {
    throw ValueError("Attempted to extract more quantity than exists.");
  }

  quantity_ -= quantity;

  Product::Ptr other(new Product(ctx_, quantity, quality_));
  tracker_.Extract(&other->tracker_);
  return other;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Resource::Ptr Product::ExtractRes(double qty) {
  return boost::static_pointer_cast<Resource>(Extract(qty));
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Product::Product(Context* ctx, double quantity, std::string quality)
    : quality_(quality),
      quantity_(quantity),
      tracker_(ctx, this),
      ctx_(ctx) {}

}  // namespace cyclus
