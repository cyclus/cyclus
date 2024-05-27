#include "res_tracker.h"

#include "recorder.h"

namespace cyclus {

ResTracker::ResTracker(Context* ctx, Resource* r)
    : tracked_(true),
      res_(r),
      ctx_(ctx),
      parent1_(0),
      parent2_(0) {}

void ResTracker::DontTrack() {
  tracked_ = false;
}

void ResTracker::Create(Agent* creator) {
  if (!tracked_) {
    return;
  }

  parent1_ = 0;
  parent2_ = 0;
  Record();
  ctx_->NewDatum("ResCreators")
      ->AddVal("ResourceId", res_->state_id())
      ->AddVal("AgentId", creator->id())
      ->Record();
}

void ResTracker::Modify() {
  if (!tracked_) {
    return;
  }

  parent1_ = res_->state_id();
  parent2_ = 0;
  Record();
}

void ResTracker::Extract(ResTracker* removed) {
  if (!tracked_) {
    return;
  }

  parent1_ = res_->state_id();
  parent2_ = 0;
  removed->parent1_ = res_->state_id();
  removed->parent2_ = 0;
  removed->tracked_ = tracked_;

  Record();
  removed->Record();
}

void ResTracker::Absorb(ResTracker* absorbed) {
  if (!tracked_) {
    return;
  }

  parent1_ = res_->state_id();
  parent2_ = absorbed->res_->state_id();
  Record();
}

void ResTracker::Package() {
  if (!tracked_) {
    return;
  }

  package_name_ = res_->package_name();
  Record();
}

void ResTracker::Record() {
  res_->BumpStateId();
  ctx_->NewDatum("Resources")
      ->AddVal("ResourceId", res_->state_id())
      ->AddVal("ObjId", res_->obj_id())
      ->AddVal("Type", res_->type())
      ->AddVal("TimeCreated", ctx_->time())
      ->AddVal("Quantity", res_->quantity())
      ->AddVal("Units", res_->units())
      ->AddVal("QualId", res_->qual_id())
      ->AddVal("PackageName", res_->package_name())
      ->AddVal("Parent1", parent1_)
      ->AddVal("Parent2", parent2_)
      ->Record();

  res_->Record(ctx_);
}

}  // namespace cyclus
