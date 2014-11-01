#ifndef CYCLUS_SRC_TOOLKIT_RES_BUF_H_
#define CYCLUS_SRC_TOOLKIT_RES_BUF_H_

#include <iomanip>
#include <limits>
#include <list>
#include <set>
#include <vector>

#include "cyc_arithmetic.h"
#include "cyc_limits.h"
#include "error.h"
#include "product.h"
#include "material.h"
#include "resource.h"

namespace cyclus {
namespace toolkit {

typedef std::vector<Resource::Ptr> ResVect;
typedef std::vector<Material::Ptr> MatVect;
typedef std::vector<Product::Ptr> ProdVect;

/// ResBuf is a helper class that provides semi-automated management of
/// a collection of resources (e.g. agent stocks and inventories).
///
/// Constructed buffers have infinite capacity unless explicitly changed.
/// Resource popping occurs in the order the resources were pushed (i.e. oldest
/// resources are popped first), unless explicitly specified otherwise.
template<class T>
class ResBuf {
 public:
  ResBuf() : cap_(INFINITY), qty_(0) {}

  virtual ~ResBuf() {}

  /// Returns the maximum resource quantity this buffer can hold (units
  /// based on constituent resource objects' units).
  /// Never throws.
  inline double cap() const {
    return cap_;
  }

  /// Sets the maximum quantity this buffer can hold (units based
  /// on constituent resource objects' units).
  ///
  /// @throws ValueError the new capacity is lower (by eps_rsrc()) than the
  /// quantity of resources that exist in the buffer.
  void cap(double cap) {
    if (qty() - cap > eps_rsrc()) {
      std::stringstream ss;
      ss << std::setprecision(17) <<"new capacity " << cap
         << " lower than existing quantity " << qty();
      throw ValueError(ss.str());
    }
    cap_ = cap;
  }

  /// Returns the total number of constituent resource objects
  /// in the buffer. Never throws.
  inline int n() const {
    return rs_.size();
  }

  /// Returns the total resource quantity of constituent resource
  /// objects in the buffer. Never throws.
  inline double qty() const {
    return qty_;
  }

  /// Returns the quantity of space remaining in this buffer.
  ///
  /// It is effectively the difference between the capacity and the quantity
  /// and is never negative. Never throws.
  inline double space() const {
    return std::max(0.0, cap_ - qty_);
  }

  /// Returns true if there are no resources in the buffer.
  inline bool empty() const {
    return rs_.empty();
  }

  /// Pops and returns the specified quantity of resources from the buffer.
  ///
  /// Resources are split if necessary in order to pop the exact quantity /
  /// requested (within eps_rsrc()).  Resources are retrieved in the order they
  /// were pushed (i.e. oldest first).
  ///
  /// @throws ValueError the specified pop quantity is larger than the
  /// buffer's current quantity.
  std::vector<typename T::Ptr> PopQty(double qty) {
    if (qty > this->qty()) {
      std::stringstream ss;
      ss << std::setprecision(17) <<"removal quantity " << qty
         << " larger than buff quantity " << this->qty();
      throw ValueError(ss.str());
    }

    std::vector<typename T::Ptr> rs;
    typename T::Ptr r;
    typename T::Ptr tmp;
    double left = qty;
    double quan;
    while (left > 0 && n() > 0) {
      r = rs_.front();
      rs_.pop_front();
      quan = r->quantity();
      if (quan > left) {
        // too big - split the res before popping
        tmp = boost::dynamic_pointer_cast<T>(r->ExtractRes(left));
        rs_.push_front(r);
        r = tmp;
      } else {
        rs_present_.erase(r);
      }

      rs.push_back(r);
      left -= quan;
    }

    UpdateQty();

    return rs;
  }


  /// Same behavior as PopQty(double) except a non-zero eps may be specified
  /// for cases where qty might be larger than the buffer's current quantity.
  std::vector<typename T::Ptr> PopQty(double qty, double eps) {
    if (qty > this->qty() + eps) {
      std::stringstream ss;
      ss << std::setprecision(17) <<"removal quantity " << qty
         << " larger than buff quantity " << this->qty();
      throw ValueError(ss.str());
    }

    if (qty >= this->qty()) {
      return PopN(n());
    }
    return PopQty(qty);
  }

  /// Pops the specified number of resource objects from the buffer.
  ///
  /// Resources are not split and are retrieved in the order they were
  /// pushed (i.e. oldest first).
  ///
  /// @throws ValueError the specified pop number is larger than the
  /// buffer's current inventoryNum or the specified number is negative.
  std::vector<typename T::Ptr> PopN(int num) {
    if (n() < num || num < 0) {
      std::stringstream ss;
      ss << "remove count " << num << " larger than buff count " << n();
      throw ValueError(ss.str());
    }

    std::vector<typename T::Ptr> rs;
    for (int i = 0; i < num; i++) {
      typename T::Ptr r = rs_.front();
      rs_.pop_front();
      rs.push_back(r);
      rs_present_.erase(r);
    }

    UpdateQty();
    return rs;
  }

  /// Same as PopQty(double) except returns the Resource-typed objects.
  std::vector<Resource::Ptr> PopQtyRes(double qty) {
    return ResCast<Resource>(PopQty(qty));
  }

  /// Same as PopQty(doble, double) except returns the Resource-typed objects.
  std::vector<Resource::Ptr> PopQtyRes(double qty, double eps) {
    return ResCast<Resource>(PopQty(qty, eps));
  }

  /// Same as PopN except returns the Resource-typed objects.
  std::vector<Resource::Ptr> PopNRes(int num) {
    return ResCast<Resource>(PopN(num));
  }

  /// Returns the next resource in line to be popped from the buffer
  /// without actually removing it from the buffer.
  typename T::Ptr Peek() {
    if (rs_.size() < 1) {
      throw ValueError("cannot peek at resource from an empty buff");
    }
    return rs_.front();
  }

  /// Pops one resource object from the buffer.
  ///
  /// Resources are not split and are retrieved by default in the order
  /// they were pushed (i.e. oldest first).
  ///
  /// @throws ValueError the buffer is empty.
  typename T::Ptr Pop() {
    if (rs_.size() < 1) {
      throw ValueError("cannot pop resource from an empty buff");
    }

    typename T::Ptr r = rs_.front();
    rs_.pop_front();
    rs_present_.erase(r);
    UpdateQty();
    return r;
  }

  /// Same as Pop, except it returns the most recently added
  /// resource.
  typename T::Ptr PopBack() {
    if (rs_.size() < 1) {
      throw ValueError("cannot pop resource from an empty buff");
    }

    typename T::Ptr r = rs_.back();
    rs_.pop_back();
    rs_present_.erase(r);
    UpdateQty();
    return r;
  }

  /// Pushs a single resource object to the buffer.
  ///
  /// Resource objects are never combined in the buffer; they are stored as
  /// unique objects. The resource object is only pushed to the buffer if it
  /// does not cause the buffer to exceed its capacity.
  ///
  /// @throws ValueError the pushing of the given resource object would
  /// cause the buffer to exceed its capacity.
  ///
  /// @throws KeyError the resource object to be pushed is already present
  /// in the buffer.
  void Push(Resource::Ptr r) {
    typename T::Ptr m = boost::dynamic_pointer_cast<T>(r);
    if (m == NULL) {
      throw CastError("pushing wrong type of resource onto ResBuf");
    } else if (r->quantity() - space() > eps_rsrc()) {
      std::stringstream ss;
      ss << "resource pushing breaks capacity limit: space=" << space()
         << ", rsrc->qty()=" << r->quantity();
      throw ValueError(ss.str());
    } else if (rs_present_.count(m) == 1) {
      throw KeyError("duplicate resource push attempted");
    }

    rs_.push_back(m);
    rs_present_.insert(m);
    UpdateQty();
  }

  /// PushAll pushes one or more resource objects (as a std::vector) to the
  /// buffer.
  ///
  /// Resource objects are never combined in the buffer; they are stored as
  /// unique objects. The resource objects are only pushed to the buffer if
  /// they do not cause the buffer to exceed its capacity; otherwise none of the
  /// given resource objects are added to the buffer.
  ///
  /// @throws ValueError adding the given resource objects would
  /// cause the buffer to exceed its capacity.
  ///
  /// @throws KeyError one or more of the resource objects to be added
  /// are already present in the buffer.
  template <class B>
  void PushAll(std::vector<B> rs) {
    std::vector<typename T::Ptr> rss;
    typename T::Ptr r;
    for (int i = 0; i < rs.size(); i++) {
      r = boost::dynamic_pointer_cast<T>(rs[i]);
      if (r == NULL) {
        throw CastError("pushing wrong type of resource onto ResBuf");
      }
      rss.push_back(r);
    }

    double tot_qty = 0;
    for (int i = 0; i < rss.size(); i++) {
      tot_qty += rss.at(i)->quantity();
    }
    if (tot_qty - space() > eps_rsrc()) {
      throw ValueError("Resource pushing breaks capacity limit.");
    }

    for (int i = 0; i < rss.size(); i++) {
      if (rs_present_.count(rss.at(i)) == 1) {
        throw KeyError("Duplicate resource pushing attempted");
      }
    }

    for (int i = 0; i < rss.size(); i++) {
      rs_.push_back(rss[i]);
      rs_present_.insert(rss[i]);
    }
    qty_ += tot_qty;
  }

 private:
  void UpdateQty() {
    typename std::list<typename T::Ptr>::iterator it;
    std::vector<double> qtys;
    for (it = rs_.begin(); it != rs_.end(); ++it) {
      qtys.push_back((*it)->quantity());
    }
    qty_ = CycArithmetic::KahanSum(qtys);
  }

  double qty_;

  /// Maximum quantity of resources this buffer can hold
  double cap_;

  /// List of constituent resource objects forming the buffer's inventory
  std::list<typename T::Ptr> rs_;
  std::set<typename T::Ptr> rs_present_;
};

}  // namespace toolkit
}  // namespace cyclus

#endif  // CYCLUS_SRC_TOOLKIT_RES_BUF_H_
