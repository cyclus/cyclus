#ifndef CYCLUS_SRC_PRODUCT_H_
#define CYCLUS_SRC_PRODUCT_H_

#include <boost/shared_ptr.hpp>

#include "context.h"
#include "resource.h"
#include "res_tracker.h"

class SimInitTest;

namespace cyclus {

/// A Product is a general type of resource in the Cyclus simulation,
/// and is a catch-all for non-standard resources.  It implements the Resource
/// class interface in a simple way usable for things such as: bananas,
/// man-hours, water, buying power, etc.
class Product : public Resource {
  friend class SimInit;
  friend class ::SimInitTest;

 public:
  typedef
  boost::shared_ptr<Product> Ptr;
  static const ResourceType kType;

  /// Creates a new product that is "live" and tracked. creator is a
  /// pointer to the agent creating the resource (usually will be the caller's
  /// "this" pointer). All future output data recorded will be done using the
  /// creator's context.
  static Ptr Create(Agent* creator, double quantity, std::string quality, std::string package_name = Package::unpackaged_name());

  /// Creates a new product that does not actually exist as part of
  /// the simulation and is untracked.
  static Ptr CreateUntracked(double quantity, std::string quality);

  /// Returns 0 (for now).
  virtual int qual_id() const {
    return qualids_[quality_];
  }

  /// Returns Product::kType.
  virtual const ResourceType type() const {
    return kType;
  }

  virtual Resource::Ptr Clone() const;

  virtual void Record(Context* ctx) const {}

  virtual std::string units() const { return "NONE"; }

  virtual double quantity() const {
    return quantity_;
  }

  /// Returns the quality of this resource (e.g. bananas, human labor, water, etc.).
  virtual const std::string& quality() const {
    return quality_;
  }

  virtual Resource::Ptr ExtractRes(double quantity);

  /// Extracts the specified mass from this resource and returns it as a
  /// new product object with the same quality/type.
  ///
  /// @throws ValueError tried to extract more than exists.
  Product::Ptr Extract(double quantity);

  /// Absorbs the contents of the given 'other' resource into this resource.
  /// @throws ValueError 'other' resource is of different quality
  void Absorb(Product::Ptr other);

  /// Returns the package id.
  virtual std::string package_name();

  /// Changes the product's package id
  virtual void ChangePackage(std::string new_package_name = Package::unpackaged_name());

 private:
  /// @param ctx the simulation context
  /// @param quantity is a double indicating the quantity
  /// @param quality the resource quality
  Product(Context* ctx, double quantity, std::string quality,
          std::string package_name = Package::unpackaged_name());

  // map<quality, quality_id>
  static std::map<std::string, int> qualids_;
  static int next_qualid_;

  Context* ctx_;
  std::string quality_;
  double quantity_;
  ResTracker tracker_;
  std::string package_name_;
};

}  // namespace cyclus

#endif  // CYCLUS_SRC_PRODUCT_H_
