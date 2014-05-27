// resource_buff.h
#ifndef RESOURCEBUFF_H_
#define RESOURCEBUFF_H_

#include <limits>
#include <list>
#include <set>
#include <vector>

#include "cyc_limits.h"
#include "error.h"
#include "product.h"
#include "material.h"
#include "resource.h"

namespace cyclus {
namespace toolkit {

static double const kBuffInfinity = std::numeric_limits<double>::max();

typedef std::vector<Resource::Ptr> Manifest;

/// ResourceBuff is a helper function that provides semi-automated management of
/// resource buffers (e.g. agent stocks and inventories).
/// 
/// Methods that begin with a "set", "make", "push", or "pop" prefix change the
/// state/behavior of the store; other methods do not.  Default constructed
/// resource store has infinite capacity. Resource popping occurs in the order
/// the resources were pushed (i.e. oldest resources are popped first), unless
/// explicitly specified otherwise.
class ResourceBuff {
 public:
  enum AccessDir {
    FRONT,
    BACK
  };
  
  ResourceBuff() : capacity_(kBuffInfinity), qty_(0) {};

  virtual ~ResourceBuff() {};

  /// capacity returns the maximum resource quantity this store can hold (units
  /// based on constituent resource objects' units).
  /// Never throws.
  inline double capacity() const {
    return capacity_;
  };

  /// set_capacity sets the maximum quantity this store can hold (units based
  /// on constituent resource objects' units).
  ///
  /// @throws ValueError the new capacity is lower (by eps_rsrc()) than the
  /// quantity of resources that already exist in the store.
  void set_capacity(double cap);

  /// count returns the total number of constituent resource objects
  /// in the store. Never throws.
  inline int count() const {
    return mats_.size();
  };

  /// quantity returns the total resource quantity of constituent resource objects
  /// in the store. Never throws.
  inline double quantity() const {
    return qty_;
  };

  /// space returns the quantity of space remaining in this store.
  ///
  /// It is effectively the difference between the capacity and the quantity.
  /// Never throws.
  inline double space() const {
    return capacity_ - qty_;
  };

  /// returns true if there are no mats in mats_
  inline bool empty() const {
    return mats_.empty();
  };

  /// PopQty pops the specified quantity of resources from the
  /// store.
  ///
  /// Resources are split if necessary in order to pop the exact quantity
  /// specified (within eps_rsrc()).  Resources are retrieved in the order they were
  /// pushed (i.e. oldest first).
  ///
  /// @throws ValueError the specified pop quantity is larger (by
  /// eps_rsrc()) than the store's current quantity.
  Manifest PopQty(double qty);

  /// PopN pops the specified number or count of resource objects from the
  /// store.
  ///
  /// Resources are not split.  Resources are retrieved in the order they were
  /// pushed (i.e. oldest first).
  ///
  /// @throws ValueError the specified pop number is larger than the
  /// store's current inventoryNum or the specified number is negative.
  Manifest PopN(int num);

  /// Pop pops one resource object from the store.
  ///
  /// Resources are not split.  Resources are retrieved by default in the order
  /// they were pushed (i.e. oldest first).
  ///
  /// @param dir the access direction, which is the front by default
  ///
  /// @throws ValueError the store is empty.
  Resource::Ptr Pop(AccessDir dir = FRONT);

  /// A convenience method identical to Pop for auto-casting to specific
  /// Resource types.
  template <class T>
  typename T::Ptr Pop() {
    return boost::dynamic_pointer_cast<T>(Pop());
  };

  /// Push pushs a single resource object to the store.
  ///
  /// Resource objects are never combined in the store; they are stored as
  /// unique objects. The resource object is only pushed to the store if it does not
  /// cause the store to exceed its capacity
  ///
  /// @throws ValueError the pushing of the given resource object would
  /// cause the store to exceed its capacity.
  ///
  /// @throws KeyError the resource object to be pushed is already present
  /// in the store.
  void Push(Resource::Ptr r);

  /// PushAll pushess one or more resource objects (as a std::vector) to the store.
  ///
  /// Resource objects are never combined in the store; they are stored as
  /// unique objects. The resource objects are only pushed to the store if they do
  /// not cause the store to exceed its capacity; otherwise none of the given
  /// resource objects are pushed to the store.
  ///
  /// @throws ValueError the pushing of the given resource objects would
  /// cause the store to exceed its capacity.
  ///
  /// @throws KeyError one or more of the resource objects to be pushed
  /// are already present in the store.
  template <class B>
  void PushAll(std::vector<B> rs) {
    double tot_qty = 0;
    for (int i = 0; i < rs.size(); i++) {
      tot_qty += rs.at(i)->quantity();
    }
    if (tot_qty - space() > eps_rsrc()) {
      throw ValueError("Resource pushing breaks capacity limit.");
    }

    for (int i = 0; i < rs.size(); i++) {
      if (mats_present_.count(rs.at(i)) == 1) {
        throw KeyError("Duplicate resource pushing attempted");
      }
    }

    for (int i = 0; i < rs.size(); i++) {
      mats_.push_back(rs[i]);
      mats_present_.insert(rs[i]);
    }
    qty_ += tot_qty;
  };

 private:
  double qty_;

  /// maximum quantity of resources this store can hold
  double capacity_;

  /// list of constituent resource objects forming the store's inventory
  std::list<Resource::Ptr> mats_;
  std::set<Resource::Ptr> mats_present_;
};
} // namespace toolkit
} // namespace cyclus
#endif
