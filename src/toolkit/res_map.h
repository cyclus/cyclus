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
  ResMap() : dirty_quantity_(true), quantity_(0) {
    Warn<EXPERIMENTAL_WARNING>("ResMap is experimental and its API may be "
                               "subject to change");
  }

  virtual ~ResMap() {}

  typedef typename std::map<K, typename R::Ptr> map_type;
  typedef typename std::map<K, typename R::Ptr>::iterator iterator;
  typedef typename std::map<K, typename R::Ptr>::const_iterator const_iterator;

  typedef typename std::map<K, int> obj_type;
  typedef typename std::map<K, int>::iterator obj_iterator;
  typedef typename std::map<K, int>::const_iterator const_obj_iterator;

  //
  // properties
  //

  /// Returns the total number of resources in the map.
  inline int size() const { return resources_.size(); }

  /// Returns the total quantity of resources in the map.
  inline double quantity() {
    if (dirty_quantity_)
      UpdateQuantity();
    return quantity_;
  };

  obj_type& obj_ids() {
    obj_ids_.clear();
    iterator it = resources_.begin();
    for (; it != resources_.end(); ++it) {
      obj_ids_[it->first] = it->second->obj_id();
    }
    return obj_ids_;
  }

  void obj_ids(obj_type oi) {
    obj_ids_ = oi;
    dirty_quantity_ = true;
  }

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

  /// Removes all elements from the map.
  void clear() {
    resources_.clear();
    obj_ids_.clear();
    dirty_quantity_ = true;
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

  /// Sets the values of map based on their object ids. Thus the objs_ids
  /// member must be set.  This is primarily for restart capabilities and is
  /// not recomended for day-to-day use.
  void Values(std::vector<typename R::Ptr> vals) {
    std::map<int, K> lookup;
    obj_iterator oit = obj_ids_.begin();
    for (; oit != obj_ids_.end(); ++oit) {
      lookup[oit->second] = oit->first;
    }
    int i = 0;
    int n = vals.size();
    for (; i < n; ++i) {
      resources_[lookup[vals[i]->obj_id()]] = vals[i];
    }
    dirty_quantity_ = true;
  }

  /// Sets the resource values of map based on their object ids. Thus the objs_ids
  /// member must be set.  This is primarily for restart capabilities and is
  /// not recomended for day-to-day use.
  void ResValues(std::vector<Resource::Ptr> vals) {
    Values(cyclus::template ResCast<R>(vals));
  }

  /// Retrieves and removes the value associated with the provided key.
  typename R::Ptr Pop(const K key) {
    iterator it = resources_.find(key);
    if (it == resources_.end()) {
      std::stringstream ss;
      ss << "key " << key << " could not be found";
      throw KeyError(ss.str());
    }
    typename R::Ptr val = it->second;
    resources_.erase(it);
    dirty_quantity_ = true;
    return val;
  }

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

  /// Whether quantity_ should be recomputed or not.
  mutable bool dirty_quantity_;

  /// Current total quantity of all resources in the mapping.
  double quantity_;

  /// Underlying container
  map_type resources_;

  /// Object ID mapping, primarily used for restart,
  obj_type obj_ids_;
};

}  // namespace toolkit
}  // namespace cyclus

#endif  // CYCLUS_SRC_TOOLKIT_RES_MAP_H_
