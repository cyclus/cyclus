
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
    ->AddVal("ResourceId", res_->id())
    ->AddVal("AgentId", creator->id())
    ->Record();
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
  removed->parent1_ = res_->id();
  removed->parent2_ = 0;
  removed->tracked_ = tracked_;

  Record();
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
  ctx_->NewDatum("Resources")
  ->AddVal("ResourceId", res_->id())
  ->AddVal("Type", res_->type())
  ->AddVal("TimeCreated", ctx_->time())
  ->AddVal("Quantity", res_->quantity())
  ->AddVal("Units", res_->units())
  ->AddVal("StateId", res_->state_id())
  ->AddVal("Parent1", parent1_)
  ->AddVal("Parent2", parent2_)
  ->Record();

  res_->Record(ctx_);
}

} // namespace cyclus
