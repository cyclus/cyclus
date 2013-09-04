// generic_resource.h
#ifndef GENERICRESOURCE_H_
#define GENERICRESOURCE_H_

#include <boost/shared_ptr.hpp>

#include "context.h"
#include "resource.h"
#include "res_tracker.h"

namespace cyclus {

/// A Generic Resource is a general type of resource in the Cyclus simulation,
/// and is a catch-all for non-standard resources.  It implements the Resource
/// class interface in a simple way usable for things such as: bananas,
/// man-hours, water, buying power, etc.
class GenericResource : public Resource {
 public:
  typedef
  boost::shared_ptr<GenericResource> Ptr;
  static const ResourceType kType;

  /// Creates a new generic resource that is "live" and tracked. All future
  /// output data recorded will be done using the passed simulation context ctx.
  static Ptr Create(double quantity, std::string quality, std::string units, Context* ctx);

  /// Creates a new generic resource that does not actually exist as part of
  /// the simulation and is untracked.
  static Ptr CreateUntracked(double quantity, std::string quality, std::string
                             units);

  /// Returns 0 (for now).
  virtual int state_id() const {
    return 0;
  };
  // TODO: give each quality its own state_id. and have it recorded in the output db.

  /// Returns GenericResource::kType.
  virtual const ResourceType type() const {
    return kType;
  };

  virtual Resource::Ptr Clone() const;

  virtual void Record(Context* ctx) const { };

  virtual std::string units() const {
    return units_;
  };

  virtual double quantity() const {
    return quantity_;
  };

  /// Returns the quality of this resource (e.g. bananas, human labor, water, etc.).
  virtual const std::string& quality() const {
    return quality_;
  };

  virtual Resource::Ptr ExtractRes(double quantity);

  /// Extracts the specified mass from this resource and returns it as a
  /// new generic resource object with the same quality/type.
  ///
  /// @throws ValueError tried to extract more than exists.
  GenericResource::Ptr Extract(double quantity);

  /// Absorbs the contents of the given 'other' resource into this resource.
  /// @throws ValueError 'other' resource is of different units and/or quality
  void Absorb(GenericResource::Ptr other);

 private:
  /// @param quantity is a double indicating the quantity
  /// @param units is a string indicating the resource unit
  GenericResource(double quantity, std::string quality, std::string units, Context* ctx);

  Context* ctx_;
  std::string units_;
  std::string quality_;
  double quantity_;
  ResTracker tracker_;
};

} // namespace cyclus

#endif
