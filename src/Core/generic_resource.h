// GenericResource.h
#if !defined(_GENERICRESOURCE_H)
#define _GENERICRESOURCE_H

#include <boost/shared_ptr.hpp>

#include "resource.h"
#include "res_tracker.h"

namespace cyclus {

/// A Generic Resource is a general type of resource in the Cyclus
/// simulation, and is a catch-all for non-standard resources.
class GenericResource : public Resource {
 public:
  typedef boost::shared_ptr<GenericResource> Ptr;
  static const ResourceType kType;

  static Ptr Create(double quantity, std::string units);
  static Ptr CreateUntracked(double quantity, std::string units);

  virtual const int id() const;

  /// not needed/no meaning for generic resources
  virtual int state_id() const {
    return 0;
  };

  /// Returns the concrete type of this resource
  virtual const ResourceType type() const {
    return kType;
  };

  /// Returns a reference to a newly allocated copy of this resource
  virtual Resource::Ptr Clone() const;

  virtual void RecordSpecial() const { };

  /// Returns the total quantity of this resource in its base unit
  virtual std::string units() const {
    return units_;
  };

  /// Returns the total quantity of this resource in its base unit
  virtual double quantity() const {
    return quantity_;
  };

  virtual Resource::Ptr ExtractRes(double quantity);

  /// Extracts the specified mass from this resource and returns it as a
  /// new generic resource object with the same quality/type.

  /// @throws CycGenResourceOverExtract
  GenericResource::Ptr Extract(double quantity);

  /// Absorbs the contents of the given 'other' resource into this
  /// resource
  /// @throws CycGenResourceIncompatible 'other' resource is of a
  void Absorb(GenericResource::Ptr other);

 private:
  /// @param quantity is a double indicating the quantity
  /// @param units is a string indicating the resource unit
  GenericResource(double quantity, std::string units);

  std::string units_;
  double quantity_;
  ResTracker tracker_;
};

} // namespace cyclus

#endif
