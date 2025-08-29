#ifndef CYCLUS_SRC_RESOURCE_H_
#define CYCLUS_SRC_RESOURCE_H_

#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>

#include "package.h"
#include "cyc_limits.h"

class SimInitTest;

namespace cyclus {

class Context;

typedef std::string ResourceType;

/// Resource defines an abstract interface implemented by types that are
/// offered, requested, and transferred between simulation agents. Resources
/// represent the lifeblood of a simulation.
class Resource {
  friend class SimInit;
  friend class ::SimInitTest;

 public:
  typedef boost::shared_ptr<Resource> Ptr;

  Resource()
      : state_id_(nextstate_id_++), unit_value_(0.0), obj_id_(nextobj_id_++) {}

  virtual ~Resource() {}

  /// Returns the unique id corresponding to this resource object. Can be used
  /// to track and/or associate other information with this resource object.
  /// You should NOT track resources by pointer.
  const int obj_id() const { return obj_id_; }

  /// Returns the unit value of this resource.
  double UnitValue() const { return unit_value_; }

  /// Sets the unit value of this resource.
  void SetUnitValue(double unit_value) { unit_value_ = unit_value; }

  /// Returns the unique id corresponding to this resource and its current
  /// state.  All resource id's are unique - even across different resource
  /// types/implementations. Runtime tracking of resources should generally
  /// use the obj_id rather than this.
  const int state_id() const { return state_id_; }

  /// Assigns a new, unique internal id to this resource and its state. This
  /// should be called by resource implementations whenever their state changes.
  /// A call to BumpStateId is not necessarily accompanied by a change to the
  /// state id. This should NEVER be called by agents.
  void BumpStateId();

  /// Returns an id representing the specific resource implementation's internal
  /// state that is not accessible via the Resource class public interface.  Any
  /// change to the qual_id should always be accompanied by a call to
  /// BumpStateId.
  virtual int qual_id() const = 0;

  /// A unique type/name for the concrete resource implementation.
  virtual const ResourceType type() const = 0;

  /// Returns an untracked (not part of the simulation) copy of the resource.
  /// A cloned resource should never record anything in the output database.
  virtual Ptr Clone() const = 0;

  /// Records the resource's state to the output database.  This method
  /// should generally NOT record data accessible via the Resource class
  /// public methods (e.g.  qual_id, units, type, quantity).
  /// @param ctx the simulation context used to record the data.
  virtual void Record(Context* ctx) const = 0;

  /// Returns the units this resource is based in (e.g. "kg").
  virtual std::string units() const = 0;

  /// Returns the quantity of this resource with dimensions as specified by
  /// the return value of units().
  virtual double quantity() const = 0;

  /// Splits the resource and returns the extracted portion as a new resource
  /// object.  Allows for things like ResBuf and Traders to split
  /// offers/requests of arbitrary resource implementation type.
  ///
  /// @return a new resource object with same state id and quantity == quantity
  virtual Ptr ExtractRes(double quantity) = 0;

  /// To enable the Decay method to be called on any child resource, define
  /// a null op Decay method here.
  /// @param curr_time the current time for the decay oepration
  virtual void Decay(int curr_time) {
    throw Error("cannot decay resource type " + this->type());
  };

  /// To enable the Absorb method to be called on any child resource, define
  /// a null op Absorb method here.
  /// @param res pointer to a resource to be absorbed by this resource
  virtual void Absorb(Ptr res) {
    throw Error("cannot absorb resource type " + this->type());
  };

  /// Returns the package id.
  virtual std::string package_name() { return Package::unpackaged_name(); };

  virtual Ptr PackageExtract(double qty, std::string new_package_name) = 0;

  /// Changes the product's package id
  virtual void ChangePackage(
      std::string new_package_name = Package::unpackaged_name()) {};

  /// Repackages a single resource into a package. If some quantity of the
  /// resource cannot be packaged using the given packaging strategy and
  /// restrictions, the remainder is left in the resource object.
  template <class T> std::vector<typename T::Ptr> Package(Package::Ptr pkg);

 protected:
  constexpr static double kUnsetUnitValue =
      std::numeric_limits<double>::quiet_NaN();

 private:
  double unit_value_;
  static int nextstate_id_;
  static int nextobj_id_;
  int state_id_;
  // Setting the state id should only be done when extracting one resource
  void state_id(int st_id) { state_id_ = st_id; }

  int obj_id_;
};

/// Casts a vector of Resources into a vector of a specific resource type T.
template <class T>
std::vector<typename T::Ptr> ResCast(std::vector<Resource::Ptr> rs) {
  std::vector<typename T::Ptr> casted;
  for (int i = 0; i < rs.size(); ++i) {
    casted.push_back(boost::dynamic_pointer_cast<T>(rs[i]));
  }
  return casted;
}

/// Casts a Resource::Ptr into a pointer of a specific resource type T.
template <class T> typename T::Ptr ResCast(Resource::Ptr r) {
  return boost::dynamic_pointer_cast<T>(r);
}

template <class T>
std::vector<typename T::Ptr> Resource::Package(Package::Ptr pkg) {
  std::vector<typename T::Ptr> ts_pkgd;
  typename T::Ptr t_pkgd;

  std::vector<double> packages = pkg->GetFillMass(quantity());
  if (packages.size() == 0) {
    return ts_pkgd;
  }

  for (int i = 0; i < packages.size(); ++i) {
    double pkg_fill = packages[i];
    t_pkgd =
        boost::dynamic_pointer_cast<T>(PackageExtract(pkg_fill, pkg->name()));
    ts_pkgd.push_back(t_pkgd);
  }

  return ts_pkgd;
}

}  // namespace cyclus

#endif  // CYCLUS_SRC_RESOURCE_H_
