
#include "res_tracker.h"

#include "event_manager.h"

namespace cyclus {

ResTracker::ResTracker(Resource* r)
  : tracked_(true),
    res_(r),
    parent1_(0),
    parent2_(0) { }

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

  parent1_ = res_->id();
  parent2_ = 0;
  Record();
}

void ResTracker::Extract(ResTracker* removed) {
  if (!tracked_) {
    return;
  }

  parent1_ = res_->id();
  parent2_ = 0;
  Record();

  removed->tracked_ = tracked_;
  removed->parent1_ = res_->id();
  removed->parent2_ = 0;
  removed->Record();
}

void ResTracker::Absorb(ResTracker* absorbed) {
  if (!tracked_) {
    return;
  }

  parent1_ = res_->id();
  parent2_ = absorbed->res_->id();
  Record();
}

void ResTracker::Record() {
  res_->BumpId();
  EM->NewEvent("ResourceHeritage")
  ->AddVal("ID", res_->id())
  ->AddVal("Parent1", parent1_)
  ->AddVal("Parent2", parent2_)
  ->Record();

  res_->Record();
}

} // namespace cyclus
