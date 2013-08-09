
#include "res_tracker.h"

#include "event_manager.h"

namespace cyclus {

int ResTracker::nextId_ = 1;

ResTracker::ResTracker(const Resource* r)
  : tracked_(true),
    res_(r),
    parent1_(0),
    parent2_(0),
    id_(0) { }

const int ResTracker::id() const {
  return id_;
}

void ResTracker::DontTrack() {
  tracked_ = false;
}

void ResTracker::Create() {
  if (!tracked_) {
    return;
  }

  Record();
}

void ResTracker::Modify() {
  if (!tracked_) {
    return;
  }

  parent1_ = id_;
  parent2_ = 0;
  Record();
}

void ResTracker::Extract(ResTracker* removed) {
  if (!tracked_) {
    return;
  }

  parent1_ = id_;
  parent2_ = 0;
  Record();

  removed->tracked_ = tracked_;
  removed->parent1_ = id_;
  removed->parent2_ = 0;
  removed->Record();
}

void ResTracker::Absorb(ResTracker* absorbed) {
  if (!tracked_) {
    return;
  }

  parent1_ = id_;
  parent2_ = absorbed->id_;
  Record();
}

void ResTracker::BumpId() {
  id_ = nextId_;
  nextId_++;
}

void ResTracker::Record() {
  BumpId();
  EM->NewEvent("Resources")
  ->AddVal("ID", id_)
  ->AddVal("Type", res_->type())
  ->AddVal("Quantity", res_->quantity())
  ->AddVal("Parent1", parent1_)
  ->AddVal("Parent2", parent2_)
  ->AddVal("StateId", res_->state_id())
  ->Record();

  res_->RecordSpecial();
}

} // namespace cyclus
