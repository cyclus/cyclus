#ifndef RESOURCE_H_
#define RESOURCE_H_

#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>


namespace cyclus {

class Context;

typedef std::string ResourceType;

/// Resource defines an abstract interface implemented by types that are
/// offered, requested, and transferred between simulation agents. Resources
/// represent the lifeblood of a simulation.
class Resource {
  friend class SimInit;

 public:
  typedef boost::shared_ptr<Resource> Ptr;

  Resource() : id_(0) {};

  virtual ~Resource() {};

  /// Returns the unique id corresponding to this resource and its current
  /// state.  All resource id's are unique - even across different resource
  /// types/implementations.
  const int id() const {
    return id_;
  };

  /// Assigns a new, unique id to this resource and its state. This should be
  /// called by resource implementations whenever their state changes.  A call to
  /// BumpId is not always accompanied by a change to the state id.
  void BumpId();

  /// Returns an id representing the specific resource implementation's
  /// internal state that is not accessible via the Resource class public
  /// interface.  Any change to the state_id should be accompanied by a call to
  /// BumpId.
  virtual int state_id() const = 0;

  /// A unique type/name for the concrete resource implementation.
  virtual const ResourceType type() const = 0;

  /// Returns an untracked (not part of the simulation) copy of the resource.
  /// A cloned resource should never record anything in the output database.
  virtual Ptr Clone() const = 0;

  /// Records the resource's state to the output database.  This method
  /// should generally NOT record data accessible via the Resource class
  /// public methods (e.g.  state_id, units, type, quantity).
  /// @param ctx the simulation context used to record the data.
  virtual void Record(Context* ctx) const = 0;

  /// Returns the units this resource is based in (e.g. "kg").
  virtual std::string units() const = 0;

  /// Returns the quantity of this resource with dimensions as specified by
  /// the return value of units().
  virtual double quantity() const = 0;

  /// Splits the resource and returns the extracted portion as a new resource
  /// object.  Allows for things like ResourceBuff and Traders to split
  /// offers/requests of arbitrary resource implementation type.
  ///
  /// @return a new resource object with same state id and quantity == quantity
  virtual Ptr ExtractRes(double quantity) = 0;

 private:
  static int nextid_;
  int id_;
};

} // namespace cyclus
#endif
