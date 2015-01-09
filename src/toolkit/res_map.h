#ifndef CYCLUS_SRC_TOOLKIT_RES_MAP_H_
#define CYCLUS_SRC_TOOLKIT_RES_MAP_H_

#include <iomanip>
#include <limits>
#include <map>
#include <set>
#include <vector>

#include "cyc_arithmetic.h"
#include "cyc_limits.h"
#include "error.h"
#include "product.h"
#include "material.h"
#include "resource.h"
#include "res_manip.h"

namespace cyclus {
namespace toolkit {

/// ResMap container for the management of resources. It allows you to associate
/// keys with individual resources. The keys are often strings or ints and the
/// ResMap enables you to add whatever semantic meaning that you want to these keys.
/// Like the ResBuf class, ResMap is able to keep track of its total quantity of
/// resources and has a maximum capacity limit. Overfilled ResMaps will throw a
/// ValueError when their quantity() is next computed.
///
/// Typically, a ResMap will be a member variable of an archetype class.
/// Resources can be added, removed, and retrieved from it as needed.
///
/// @code
/// class HasResMapAgent : public cyclus::Facility {
///  public:
///   Tick() {
///     cyclus::Material::Ptr best = inventory_["best"];
///     double invsize = inventory_.quantity();
///   }
///
///  private:
///   cyclus::toolkit::ResMap<std::string, cyclus::Material> inventory_;
/// };
/// @endcode
template <class K, class R>
class ResMap {
 public:
  ResMap() : capacity_(INFINITY), dirty_quantity_(true), quantity_(0) {
    Warn<EXPERIMENTAL_WARNING>("ResMap is experimental and its API may be "
                               "subject to change");
  }

  virtual ~ResMap() {}

  typedef typename std::map<K, typename R::Ptr> map_type;
  typedef typename std::map<K, typename R::Ptr>::iterator iterator;
  typedef typename std::map<K, typename R::Ptr>::const_iterator const_iterator;

  /// Returns the maximum resource quantity this mapping can hold (units
  /// based on constituent resource objects' units).
  inline double capacity() const { return capacity_; }

  /// Sets the maximum quantity this mapping can hold (units based
  /// on constituent resource objects' units).
  ///
  /// @throws ValueError the new capacity is lower (by eps_rsrc()) than the
  /// quantity of resources that exist in the buffer.
  void capacity(double cap) {
    if (quantity() - cap > eps_rsrc()) {
      std::stringstream ss;
      ss << std::setprecision(17) << "new capacity " << cap
         << " lower than existing quantity " << quantity();
      throw ValueError(ss.str());
    }
    capacity_ = cap;
  }

  /// Returns the total number of resources in the map.
  inline int size() const { return resources_.size(); }

  /// Returns the total quantity of resources in the map.
  inline double quantity() {
    if (dirty_quantity_)
      UpdateQuantity();
    if (quantity_ > capacity_ + eps_rsrc()) {
      std::stringstream ss;
      ss << std::setprecision(17) << "quantity " << quantity_
         << " greater than than allowed capacity " << capacity_;
      throw ValueError(ss.str());
    }
    return quantity_;
  };

  /// Returns the quantity of space remaining in the mapping.
  /// This is effectively the difference between the capacity and the quantity
  /// and is non-negative.
  inline double space() { return std::max(0.0, capacity_ - quantity()); }

  /// Returns true if there are no resources in the map.
  inline bool empty() const { return resources_.empty(); }

  //
  // std::map interface
  //

  /// Returns a reference to a resource pointer given a key.
  typename R::Ptr& operator[](const K& k) {
    dirty_quantity_ = true;
    return resources_[k];
  };

  /// Returns a reference to a resource pointer given a key.
  const typename R::Ptr& operator[](const K& k) const {
    dirty_quantity_ = true;
    return const_cast<map_type&>(resources_)[k];
  };

  /// Returns an iterator to the begining of the map.
  iterator begin() {
    dirty_quantity_ = true;
    return resources_.begin();
  }

  /// Returns a const iterator to the begining of the map.
  const_iterator begin() const { return resources_.begin(); }

  /// Returns a const iterator to the begining of the map.
  const_iterator cbegin() const { return resources_.begin(); }

  /// Returns an iterator to the end of the map.
  iterator end() {
    dirty_quantity_ = true;
    return resources_.end();
  }

  /// Returns a const iterator to the end of the map.
  const_iterator end() const { return resources_.end(); }

  /// Returns a const iterator to the end of the map.
  const_iterator cend() const { return resources_.end(); }

  /// Removes an element at a given position in the map.
  void erase(iterator position) {
    resources_.erase(position);
    UpdateQuantity();
  };

  /// Removes an element from the map, given its key.
  typename map_type::size_type erase(const K& k) {
    typename map_type::size_type s = resources_.erase(k);
    UpdateQuantity();
    return s;
  };

  /// Removes elements along a range from the first to last position in the map.
  void erase(iterator first, iterator last) {
    resources_.erase(first, last);
    UpdateQuantity();
  };

  //
  // Non-std::map interface 
  //

  /// Returns a vector of the values in the map
  std::vector<typename R::Ptr> Values() {
    int i = 0;
    int n = resources_.size();
    std::vector<typename R::Ptr> vals (n);
    iterator it = resources_.begin();
    while (it != resources_.end()) {
      vals[i] = it->second;
      ++i;
      ++it;
    }
    return vals;
  }

  /// Returns a vector resource pointers for the values in the map
  std::vector<Resource::Ptr> ResValues() { return ResCast(Values()); }

 private:
  /// Recomputes the internal quantity variable.
  void UpdateQuantity() {
    using std::vector;
    iterator it = resources_.begin();
    int i = 0;
    int n = resources_.size();
    if (n == 0) {
      quantity_ = 0.0;
    } else {
      vector<double> qtys (n, 0.0);
      while (it != resources_.end()) {
        qtys[i] = (*(it->second)).quantity();
        ++i;
        ++it;
      }
      quantity_ = CycArithmetic::KahanSum(qtys);
    }
    dirty_quantity_ = false;
  }

  /// Maximum quantity of resources this mapping can hold.
  double capacity_;

  /// Whether quantity_ should be recomputed or not.
  bool dirty_quantity_;

  /// Current total quantity of all resources in the mapping.
  double quantity_;

  /// Underlying container
  map_type resources_;
};

}  // namespace toolkit
}  // namespace cyclus

#endif  // CYCLUS_SRC_TOOLKIT_RES_MAP_H_
