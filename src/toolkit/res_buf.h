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
#include "res_manip.h"

namespace cyclus {
namespace toolkit {

typedef std::vector<Resource::Ptr> ResVec;
typedef std::vector<Material::Ptr> MatVec;
typedef std::vector<Product::Ptr> ProdVec;

/// ResBuf is a helper class that provides semi-automated management of
/// a collection of resources (e.g. agent stocks and inventories).
/// Constructed buffers have infinite capacity unless explicitly changed.
/// Resource popping occurs in the order the resources were pushed (i.e. oldest
/// resources are popped first), unless explicitly specified otherwise.
///
/// Typically, a ResBuf will be a member variable on an agent/archetype class.
/// Resources can be added and retrieved from it as needed, and the buffer can
/// be queried in various ways as done in the example below:
///
/// @code
/// class MyAgent : public cyclus::Facility {
///  public:
///   Tick() {
///     double batch_size = 2703;
///     if (outventory_.space() < batch_size) {
///       return;
///     } else if (inventory_.quantity() < batch_size) {
///       return;
///     }
///
///     outventory_.Push(inventory_.Pop(batch_size));
///   }
///
///   ... // resource exchange to fill up inventory_ buffer
///
///  private:
///   ...
///   cyclus::toolkit::ResBuf<cyclus::Material> inventory_;
///   cyclus::toolkit::ResBuf<cyclus::Material> outventory_;
/// };
/// @endcode
///
/// In this example, if there is sufficient material in inventory_, 2703 kg is
/// removed as a single object that is then placed in another buffer
/// (outventory_) each time step.
template <class T>
class ResBuf {
 public:
  ResBuf(bool is_bulk=false, bool unpackaged=true) : cap_(INFINITY), qty_(0), is_bulk_(is_bulk), unpackaged_(unpackaged) { }

  virtual ~ResBuf() {}

  /// Returns the maximum resource quantity this buffer can hold (units
  /// based on constituent resource objects' units).
  /// Never throws.
  inline double capacity() const { return cap_; }

  /// Sets the maximum quantity this buffer can hold (units based
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
    cap_ = cap;
  }

  /// Returns the total number of constituent resource objects
  /// in the buffer. Never throws.
  inline int count() const { return rs_.size(); }

  /// Returns the total resource quantity of constituent resource
  /// objects in the buffer. Never throws.
  inline double quantity() const { return qty_; }

  /// Returns the quantity of space remaining in this buffer.
  /// This is effectively the difference between the capacity and the quantity
  /// and is never negative. Never throws.
  inline double space() const { return std::max(0.0, cap_ - qty_); }

  /// Returns true if there are no resources in the buffer.
  inline bool empty() const { return rs_.empty(); }

  /// Pops and returns the specified quantity from the buffer as a vector of 
  /// resources.
  /// Resources are split if necessary in order to pop the exact quantity
  /// requested (within eps_rsrc()).  Resources are retrieved in the order they
  /// were pushed (i.e. oldest first).
  ///
  /// @throws ValueError the specified pop quantity is larger than the
  /// buffer's current inventory.
  std::vector<typename T::Ptr> PopVector(double qty) {
    if (qty > this->quantity()) {
      std::stringstream ss;
      ss << std::setprecision(17) << "removal quantity " << qty
         << " larger than buff quantity " << this->quantity();
      throw ValueError(ss.str());
    }

    std::vector<typename T::Ptr> rs;
    typename T::Ptr r;
    typename T::Ptr tmp;
    double left = qty;
    double quan;
    while (left > 0 && count() > 0) {
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

      qty_ -= r->quantity();
      rs.push_back(r);
      left -= quan;
    }

    UpdateQty();

    return rs;
  }

  /// Pops and returns the specified quantity from the buffer as a single
  /// resource object.
  /// Resources are split if necessary in order to pop the exact quantity
  /// requested (within eps_rsrc()).  Resources are retrieved in the order they
  /// were pushed (i.e. oldest first) and are squashed into a single object
  /// when returned.
  typename T::Ptr Pop(double qty) {
    return Squash(PopVector(qty));
  }

  /// Same behavior as Pop(double) except a non-zero eps may be specified.  eps
  /// is used only in cases where qty might be slightly larger than the
  /// buffer's current inventory quantity.
  typename T::Ptr Pop(double qty, double eps) {
    if (qty > this->quantity() + eps) {
      std::stringstream ss;
      ss << std::setprecision(17) << "removal quantity " << qty
         << " larger than buff quantity " << this->quantity();
      throw ValueError(ss.str());
    }

    if (qty >= this->quantity()) {
      return Squash(PopN(count()));
    }
    return Pop(qty);
  }  

  /// Pops the specified number of resource objects from the buffer.
  /// Resources are not split and are retrieved in the order they were
  /// pushed (i.e. oldest first).
  ///
  /// @throws ValueError the specified n is larger than the
  /// buffer's current resource count or the specified number is negative.
  std::vector<typename T::Ptr> PopN(int n) {
    if (count() < n || n < 0) {
      std::stringstream ss;
      ss << "remove count " << n << " larger than buff count " << count();
      throw ValueError(ss.str());
    }

    std::vector<typename T::Ptr> rs;
    for (int i = 0; i < n; i++) {
      typename T::Ptr r = rs_.front();
      qty_ -= r->quantity();
      rs_.pop_front();
      rs.push_back(r);
      rs_present_.erase(r);
    }

    UpdateQty();
    return rs;
  }

  /// Same as PopN except returns the Resource-typed objects.
  ResVec PopNRes(int n) { return ResCast(PopN(n)); }

  /// Returns the next resource in line to be popped from the buffer
  /// without actually removing it from the buffer.
  typename T::Ptr Peek() {
    if (rs_.size() < 1) {
      throw ValueError("cannot peek at resource from an empty buff");
    }
    return rs_.front();
  }

  /// Pops one resource object from the buffer.
  /// Resources are not split and are retrieved in the order
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
    qty_ -= r->quantity();
    UpdateQty();
    return r;
  }

  /// Same as Pop, except it returns the most recently added resource.
  typename T::Ptr PopBack() {
    if (rs_.size() < 1) {
      throw ValueError("cannot pop resource from an empty buff");
    }

    typename T::Ptr r = rs_.back();
    rs_.pop_back();
    rs_present_.erase(r);
    qty_ -= r->quantity();
    UpdateQty();
    return r;
  }

  /// Pushes a single resource object to the buffer. If not classified as a bulk
  /// storage buffer, resource objects are not combined in the buffer; they
  /// are stored as unique objects. The resource object is only pushed to the
  /// buffer if it does not cause the buffer to exceed its capacity.
  ///
  /// @throws ValueError the pushing of the given resource object would cause
  /// the buffer to exceed its capacity.
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
         << ", rsrc->quantity()=" << r->quantity();
      throw ValueError(ss.str());
    } else if (rs_present_.count(m) == 1) {
      throw KeyError("duplicate resource push attempted");
    }

    if (!is_bulk_  || rs_.size() == 0) {
      // strip package id and set as default
      if (unpackaged_) {
        m->ChangePackage();
      }
      rs_.push_back(m);
      rs_present_.insert(m);
    } else {
      rs_.front()->Absorb(m);
    }
    qty_ += r->quantity();
    UpdateQty();
  }

  /// Pushes one or more resource objects (as a std::vector) to the buffer. If
  /// not classified as a bulk storage buffer, resource objects are not
  /// squashed in the buffer; they are stored as unique objects. The resource
  /// objects are only pushed to the buffer if they do not cause the buffer to
  /// exceed its capacity; otherwise none of the given resource objects are
  /// added to the buffer.
  ///
  /// @throws ValueError adding the given resource objects would cause the
  /// buffer to exceed its capacity.
  ///
  /// @throws KeyError one or more of the resource objects to be added are
  /// already present in the buffer.
  template <class B>
  void Push(std::vector<B> rs) {
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
      if (!is_bulk_ || rs_.size() == 0) {
        if (unpackaged_) {
          rss[i]->ChangePackage();
        }
        rs_.push_back(rss[i]);
        rs_present_.insert(rss[i]);
      } else {
        rs_.front()->Absorb(rss[i]);
      }
    }
    qty_ += tot_qty;
  }

  /// Decays all the materials in a resource buffer
  /// @param curr_time time to calculate decay inventory
  ///        (default: -1 uses the current time of the context)
  void Decay(int curr_time = -1) {
    for (int i = 0; i < rs_.size(); i++) {
      rs_.at(i)->Decay(curr_time);
    }
  }

 private:
  void UpdateQty() {
    int n = rs_.size();
    if (n == 0) {
      qty_ = 0;
    } else if (n == 1) {
      qty_ = rs_.front()->quantity();
    }
  }

  double qty_;

  /// Maximum quantity of resources this buffer can hold
  double cap_;

  /// Whether materials should be stored as a single squashed item or as individual resource objects
  bool is_bulk_;
  /// Whether materials should be stripped of their packaging before being
  /// pushed onto the resbuf. If res_buf is bulk, this is assumed true.
  bool unpackaged_;

  /// List of constituent resource objects forming the buffer's inventory
  std::list<typename T::Ptr> rs_;
  std::set<typename T::Ptr> rs_present_;
};

}  // namespace toolkit
}  // namespace cyclus

#endif  // CYCLUS_SRC_TOOLKIT_RES_BUF_H_
