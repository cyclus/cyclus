#ifndef RESOURCE_H_
#define RESOURCE_H_

#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>

namespace cyclus {

typedef std::string ResourceType;

class Resource {
 public:
  typedef boost::shared_ptr<Resource> Ptr;

  virtual ~Resource() { };

  /// returns the unique id corresponding to this resource and its current
  /// state.
  const int id() const {
    return id_;
  };

  /// assigns a new, unique id to this resource and its state.
  void BumpId();

  /// returns an id representing the specific resource implementation's
  /// internal state.  Any change to the state_id should be accompanied by a
  /// call to BumpId.
  virtual int state_id() const = 0;

  virtual const ResourceType type() const = 0;

  /// returns an untracked (not part of the simulation) copy of the resource.
  virtual Ptr Clone() const = 0;
  // the clone method implementations should set tracked_ = false.

  /// records the resource's state to the output database.  This method should
  /// NOT record data accessible via the Resource class public methods (e.g.
  /// state_id, units, type, quantity)
  virtual void Record() const = 0;

  /// Returns the units this resource is based in.
  virtual std::string units() const = 0;

  /// returns the quantity of this resource with dimensions as specified by
  /// units().
  virtual double quantity() const = 0;

  /// splits the resource and returns the extracted portion as a new resource
  /// object.  Allows for things like ResourceBuff and market matching to
  /// split offers/requests of arbitrary resource implementation type.
  virtual Ptr ExtractRes(double quantity) = 0;

 private:
  static int nextid_;
  int id_;
};

} // namespace cyclus
#endif
