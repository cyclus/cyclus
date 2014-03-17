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
  friend class SimInit;

 public:
  typedef
  boost::shared_ptr<GenericResource> Ptr;
  static const ResourceType kType;

  /// Creates a new generic resource that is "live" and tracked. creator is a
  /// pointer to the model creating the resource (usually will be the caller's
  /// "this" pointer). All future output data recorded will be done using the
  /// creator's context.
  static Ptr Create(Agent* creator, double quantity, std::string quality);
                    

  /// Creates a new generic resource that does not actually exist as part of
  /// the simulation and is untracked.
  static Ptr CreateUntracked(double quantity, std::string quality);

  /// Returns 0 (for now).
  virtual int state_id() const {
    return 0;
  };

  /// Returns GenericResource::kType.
  virtual const ResourceType type() const {
    return kType;
  };

  virtual Resource::Ptr Clone() const;

  virtual void Record(Context* ctx) const {};

  virtual std::string units() const {return "NONE";};

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
  /// @throws ValueError 'other' resource is of different quality
  void Absorb(GenericResource::Ptr other);

 private:
  /// @param ctx the simulation context
  /// @param quantity is a double indicating the quantity
  /// @param quality the resource quality
  GenericResource(Context* ctx, double quantity, std::string quality);

  // map<quality, quality_id>
  static std::map<std::string, int> stateids_;
  static int next_state_;

  Context* ctx_;
  std::string quality_;
  double quantity_;
  ResTracker tracker_;
};

} // namespace cyclus

#endif
