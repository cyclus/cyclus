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

  /// Unique for each material object.  Changes whenever *any* state changing
  /// operation is made.
  virtual const int id() const = 0;

  /// returns an id representing the specific resource implementation's internal state.
  virtual int state_id() const = 0;

  virtual const ResourceType type() const = 0;

  /// returns an untracked (not part of the simulation) copy of the resource.
  virtual Ptr Clone() const = 0;
  // the clone method implementations should set tracked_ = false.

  /// records the resource's state that is not accessible via the Resource /
  /// class interface (e.g. don't record units, quantity, etc) in its own
  /// table.
  virtual void RecordSpecial() const = 0;

  /// Returns the units this resource is based in.
  virtual std::string units() const = 0;

  /// returns the quantity of this resource with dimensions as specified by units().
  virtual double quantity() const = 0;

  /// splits the resource and returns the extracted portion as a new resource
  /// object.  Allows for things like ResourceBuff and market matching to
  /// split offers/requests of arbitrary resource implementation type.
  virtual Ptr ExtractRes(double quantity) = 0;

};

} // namespace cyclus
#endif
