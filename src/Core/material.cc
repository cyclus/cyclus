
#include "material.h"

#include "comp_math.h"
#include "error.h"
#include "logger.h"
#include "timer.h"

namespace cyclus {

const ResourceType Material::kType = "Material";
std::map<Material*, bool> Material::all_mats_;

Material::~Material() {
  all_mats_.erase(this);
}

Material::Ptr Material::Create(double quantity, Composition::Ptr c) {
  Material::Ptr m(new Material(quantity, c));
  m->tracker_.Create();
  return m;
}

Material::Ptr Material::CreateUntracked(double quantity, Composition::Ptr c) {
  Material::Ptr m(new Material(quantity, c));
  m->tracker_.DontTrack();
  return m;
}

int Material::state_id() const {
  return comp_->id();
}

const ResourceType Material::type() const {
  return Material::kType;
}

Resource::Ptr Material::Clone() const {
  Material* m = new Material(*this);
  Resource::Ptr c(m);
  m->tracker_.DontTrack();
  return c;
}

void Material::Record() const {
  comp_->Record();
}

std::string Material::units() const {
  return "kg";
}

double Material::quantity() const {
  return qty_;
}

Resource::Ptr Material::ExtractRes(double qty) {
  return boost::static_pointer_cast<Resource>(ExtractQty(qty));
}

Material::Ptr Material::ExtractQty(double qty) {
  return ExtractComp(qty, comp_);
}

Material::Ptr Material::ExtractComp(double qty, Composition::Ptr c,
                                    double threshold) {
  if (qty_ < qty) {
    throw ValueError("mass extraction causes negative quantity");
  }

  if (comp_ != c) {
    CompMap v(comp_->mass());
    compmath::Normalize(&v, qty_);
    CompMap otherv(c->mass());
    compmath::Normalize(&otherv, qty);
    CompMap newv = compmath::Sub(v, otherv);
    compmath::ApplyThreshold(&newv, threshold);
    comp_ = Composition::CreateFromMass(newv);
  }

  qty_ -= qty;

  Material::Ptr other(new Material(qty, c));

  tracker_.Extract(&other->tracker_);

  return other;
}

void Material::Absorb(Material::Ptr mat) {
  if (comp_ != mat->comp()) {
    CompMap v(comp_->mass());
    compmath::Normalize(&v, qty_);
    CompMap otherv(mat->comp()->mass());
    compmath::Normalize(&otherv, mat->quantity());
    comp_ = Composition::CreateFromAtom(compmath::Add(v, otherv));
  }
  qty_ += mat->qty_;
  mat->qty_ = 0;

  tracker_.Absorb(&mat->tracker_);
}

void Material::Transmute(Composition::Ptr c) {
  comp_ = c;
  tracker_.Modify();
}

void Material::Decay(int curr_time) {
  int dt = curr_time - prev_decay_time_;
  prev_decay_time_ = curr_time;
  if (dt > 0) {
    Transmute(comp_->Decay(dt));
  }
}

void Material::DecayAll(int curr_time) {
  std::map<Material*, bool>::iterator it;
  for (it = all_mats_.begin(); it != all_mats_.end(); ++it) {
    it->first->Decay(curr_time);
  }
}

Composition::Ptr Material::comp() const {
  return comp_;
}

Material::Material(double quantity, Composition::Ptr c)
  : qty_(quantity), comp_(c), tracker_(this) {
  all_mats_[this] = true;
  prev_decay_time_ = TI->time();
}

} // namespace cyclus
