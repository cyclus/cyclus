
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

const int Material::id() const {
  return tracker_.id();
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

void Material::RecordSpecial() const {
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

Material::Ptr Material::ExtractComp(double qty, Composition::Ptr c) {
  if (qty_ < qty) {
    throw ValueError("mass extraction causes negative quantity");
  }

  Composition::Vect v = compmath::Sub(comp_->atom_vect(), qty_, c->atom_vect(), qty);
  comp_ = Composition::CreateFromAtom(v);
  qty_ -= qty;

  Material::Ptr other(new Material(qty, c));

  tracker_.Extract(&other->tracker_);

  return other;
}

void Material::Absorb(Material::Ptr mat) {
  Composition::Vect v = compmath::Add(comp_->atom_vect(), qty_,
                                      mat->comp()->atom_vect(), mat->quantity());
  comp_ = Composition::CreateFromAtom(v);
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
