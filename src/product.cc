#include "product.h"

#include "error.h"
#include "logger.h"

namespace cyclus {

const ResourceType Product::kType = "Product";

std::map<std::string, int> Product::qualids_;
int Product::next_qualid_ = 1;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Product::Ptr Product::Create(Agent* creator, double quantity,
                             std::string quality, int package_id) {
  if (qualids_.count(quality) == 0) {
    qualids_[quality] = next_qualid_++;
    creator->context()->NewDatum("Products")
        ->AddVal("QualId", qualids_[quality])
        ->AddVal("Quality", quality)
        ->AddVal("PackageId", package_id)
        ->Record();
  }

  // the next lines must come after qual id setting
  Product::Ptr r(new Product(creator->context(), quantity, quality, package_id));
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

  Product::Ptr other(new Product(ctx_, quantity, quality_, package_id_));
  tracker_.Extract(&other->tracker_);
  return other;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Resource::Ptr Product::ExtractRes(double qty) {
  return boost::static_pointer_cast<Resource>(Extract(qty));
}

int Product::package_id() {
  return package_id_;
}

void Product::ChangePackageId(int new_package_id) {
  if (new_package_id == package_id_ || ctx_ == NULL) {
    // no change needed
    return;
  }
  else if (new_package_id == Package::unpackaged_id()) {
    // unpackaged has functionally no restrictions
    package_id_ = new_package_id;
    return;
  }
  
  Package::Ptr p = ctx_->GetPackageById(package_id_);
  double min = p->fill_min();
  double max = p->fill_max();
  if (quantity_ >= min && quantity_ <= max) {
    package_id_ = new_package_id;
  } else {
    throw ValueError("Product quantity is outside of package fill limits.");
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Product::Product(Context* ctx, double quantity, std::string quality, int package_id)
    : quality_(quality),
      quantity_(quantity),
      tracker_(ctx, this),
      ctx_(ctx),
      package_id_(package_id) {}

}  // namespace cyclus
