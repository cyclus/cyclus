#ifndef CYCLUS_SRC_TOOLKIT_TOTAL_INV_TRACKER_H_
#define CYCLUS_SRC_TOOLKIT_TOTAL_INV_TRACKER_H_

#include "error.h"
#include "res_buf.h"

namespace cyclus {
namespace toolkit {

/// TotalInvTracker is a helper class that tracks the total inventory of
/// multiple ResBufs. This can be useful when a single agent uses multiple
/// buffers to manage their process, but a limit should be placed on their
/// combined  inventory, such as a agent- or facility-wide maximum inventory.
/// Like  ResBufs, TotalInvTracker has an infinite capacity unless explicitly
/// changed.
///
/// TotalInvTracker does not hold any inventory itself, it only tracks the
/// quantities of other ResBufs. TotalInvTracker currently tracks quantities
/// alone, not compositions.
///
/// @code
/// class MyAgent : public cyclus:: Facility {
///   public:
///     EnterNotify() {
///       cyclus::Facility::EnterNotify();
///       tracker_.Init({&inventory_, &outventory_}, facility_max_inv_size_);
///
///       cyclus::MatlBuyPolicy p;
///       p.Init(this, &inventory_, "inventory", &tracker_);
/// }
///   protected:
///     cyclus::toolkit::ResBuf<cyclus::Material> inventory_;
///     cyclus::toolkit::ResBuf<cyclus::Material> outventory_;
///
///     double facility_max_inv_size_ = 1000;
///     cyclus::toolkit::TotalInvTracker tracker_;
/// }

class TotalInvTracker {
 public:
  /// Creates an uninitialized tracker. The Init function MUST be called before
  /// the tracker is used.
  TotalInvTracker()
      : max_inv_size_(std::numeric_limits<double>::max()), qty_(0) {};

  TotalInvTracker(std::vector<ResBuf<Material>*> bufs,
                  double max_inv_size = std::numeric_limits<double>::max()) {
    Init(bufs, max_inv_size);
  }

  ~TotalInvTracker() {};

  /// Initializes the tracker with the given ResBufs. The tracker will have
  /// infinite capacity unless explicitly changed.
  void Init(std::vector<ResBuf<Material>*> bufs,
            double max_inv_size = std::numeric_limits<double>::max()) {
    if (bufs.size() == 0) {
      throw ValueError(
          "TotalInvTracker must be initialized with at least one ResBuf");
    }
    bufs_ = bufs;
    if (max_inv_size <= 0) {
      throw ValueError(
          "TotalInvTracker must be initialized with a positive capacity");
    }
    max_inv_size_ = max_inv_size;
  }

  /// Returns the total quantity of all tracked ResBufs.
  /// @throws ValueError if the tracker has not been initialized (zero)
  inline double quantity() {
    int num = num_bufs();
    qty_ = 0;
    for (int i = 0; i < num; i++) {
      qty_ += bufs_[i]->quantity();
    }
    return qty_;
  }

  /// Returns the total capacity that could go in the ResBufs. Either the
  /// capacity of the TotalInvTracker, or the sum of each ResBuf's capacity,
  /// whichever is lower.
  inline double capacity() {
    return std::min(total_capacity_bufs(), max_inv_size_);
  }

  // Returns the sum of the capacities of all buffers. Does not include the
  // capacity of the tracker
  inline double total_capacity_bufs() {
    int num = num_bufs();
    double cap = 0;
    for (int i = 0; i < num; i++) {
      cap += bufs_[i]->capacity();
    }
    return cap;
  }

  /// Returns the total capacity of the traker. Does not include ResBufs
  inline double tracker_capacity() { return max_inv_size_; }

  /// Returns the remaining facility-wide space across all tracked ResBufs.
  inline double space() { return std::max(0.0, capacity() - quantity()); }

  /// Returns the remaining space in the given ResBuf, considering the
  /// facility-wide limitations.
  inline double constrained_buf_space(ResBuf<Material>* buf) {
    return std::min(buf->space(), space());
  }

  /// Returns true if there are no resources in any buffer
  inline bool empty() { return quantity() == 0; }

  /// Returns number of buffers being tracked
  /// @throws ValueError if the tracker has not been initialized (zero)
  inline int num_bufs() {
    int num = bufs_.size();
    if (num == 0) {
      throw ValueError(
          "TotalInvTracker has not been initialized, no buffers to track");
    }
    return num;
  }

  /// Change the total capacity across all ResBufs. The new capacity must be
  /// greater than the current quantity.
  /// @throws ValueError if the new capacity is less than the current quantity
  void set_capacity(double cap) {
    if (quantity() - cap > eps_rsrc()) {
      std::stringstream ss;
      ss << std::setprecision(17) << "new capacity " << cap
         << " lower than existing quantity " << quantity();
      throw ValueError(ss.str());
    }
    max_inv_size_ = cap;
  }

  bool buf_in_tracker(ResBuf<Material>* buf) {
    for (int i = 0; i < num_bufs(); i++) {
      if (bufs_[i] == buf) {
        return true;
      }
    }
    return false;
  }

 private:
  double max_inv_size_;
  double qty_;
  std::vector<ResBuf<Material>*> bufs_;
};

}  // namespace toolkit
}  // namespace cyclus

#endif  // CYCLUS_SRC_TOOLKIT_TOTAL_INV_TRACKER_H_