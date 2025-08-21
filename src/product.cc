#include "product.h"

#include "error.h"
#include "logger.h"
#include "cyc_limits.h"

namespace cyclus {

const ResourceType Product::kType = "Product";

std::map<std::string, int> Product::qualids_;
int Product::next_qualid_ = 1;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Product::Ptr Product::Create(Agent* creator, double quantity,
                             std::string quality, std::string package_name,
                             double unit_value) {
  if (qualids_.count(quality) == 0) {
    qualids_[quality] = next_qualid_++;
    creator->context()
        ->NewDatum("Products")
        ->AddVal("QualId", qualids_[quality])
        ->AddVal("Quality", quality)
        ->Record();
  }

  // the next lines must come after qual id setting
  Product::Ptr r(new Product(creator->context(), quantity, quality,
                             package_name, unit_value));
  r->tracker_.Create(creator);
  return r;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Product::Ptr Product::CreateUntracked(double quantity, std::string quality) {
  Product::Ptr r(
      new Product(NULL, quantity, quality, Package::unpackaged_name(), kUnsetUnitValue));
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
  double tot_qty = quantity_ + other->quantity();
  double avg_unit_value =
      (quantity_ * UnitValue() + other->quantity() * other->UnitValue()) /
      tot_qty;
  SetUnitValue(avg_unit_value);
  quantity_ = tot_qty;
  other->quantity_ = 0;

  tracker_.Absorb(&other->tracker_);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Product::Ptr Product::Extract(double quantity) {
  if (quantity > quantity_) {
    throw ValueError("Attempted to extract more quantity than exists.");
  }

  quantity_ -= quantity;

  Product::Ptr other(
      new Product(ctx_, quantity, quality_, package_name_, UnitValue()));
  tracker_.Extract(&other->tracker_);
  return other;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Resource::Ptr Product::ExtractRes(double qty) {
  return boost::static_pointer_cast<Resource>(Extract(qty));
}

std::string Product::package_name() {
  return package_name_;
}

// Not sure what to do here with the unit value and packaging...
Resource::Ptr Product::PackageExtract(double qty,
                                      std::string new_package_name) {
  if (qty > quantity_) {
    throw ValueError("Attempted to extract more quantity than exists.");
  }

  quantity_ -= qty;
  Product::Ptr other(
      new Product(ctx_, qty, quality_, new_package_name, UnitValue()));

  // this call to res_tracker must come first before the parent resource
  // state id gets modified
  other->tracker_.Package(&tracker_);
  if (quantity_ > cyclus::eps_rsrc()) {
    tracker_.Modify();
  }
  return boost::static_pointer_cast<Resource>(other);
}

void Product::ChangePackage(std::string new_package_name) {
  if (new_package_name == package_name_ || ctx_ == NULL) {
    // no change needed
    return;
  } else if (new_package_name == Package::unpackaged_name()) {
    // unpackaged has functionally no restrictions
    package_name_ = new_package_name;
    tracker_.Package();
    return;
  }
  Package::Ptr p = ctx_->GetPackage(new_package_name);
  double min = p->fill_min();
  double max = p->fill_max();
  if (quantity_ >= min && quantity_ <= max) {
    package_name_ = new_package_name;
    tracker_.Package();
  } else {
    throw ValueError("Product quantity is outside of package fill limits.");
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Product::Product(Context* ctx, double quantity, std::string quality,
                 std::string package_name, double unit_value)
    : quality_(quality),
      quantity_(quantity),
      tracker_(ctx, this),
      ctx_(ctx),
      package_name_(package_name) {
  SetUnitValue(unit_value);
}

}  // namespace cyclus
