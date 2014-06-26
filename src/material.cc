#include "material.h"

#include <math.h>

#include "comp_math.h"
#include "context.h"
#include "decayer.h"
#include "error.h"
#include "logger.h"

namespace cyclus {

const ResourceType Material::kType = "Material";

Material::~Material() {}

Material::Ptr Material::Create(Agent* creator, double quantity,
                               Composition::Ptr c) {
  Material::Ptr m(new Material(creator->context(), quantity, c));
  m->tracker_.Create(creator);
  return m;
}

Material::Ptr Material::CreateUntracked(double quantity,
                                        Composition::Ptr c) {
  Material::Ptr m(new Material(NULL, quantity, c));
  return m;
}

int Material::qual_id() const {
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

void Material::Record(Context* ctx) const {
  // Note that no time field is needed because the resource ID changes
  // every time the resource changes - state_id by itself is already unique.
  ctx_->NewDatum("MaterialInfo")
      ->AddVal("ResourceId", state_id())
      ->AddVal("PrevDecayTime", prev_decay_time_)
      ->Record();

  comp_->Record(ctx);
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

  Material::Ptr other(new Material(ctx_, qty, c));

  tracker_.Extract(&other->tracker_);

  return other;
}

void Material::Absorb(Material::Ptr mat) {
  if (comp_ != mat->comp()) {
    CompMap v(comp_->mass());
    compmath::Normalize(&v, qty_);
    CompMap otherv(mat->comp()->mass());
    compmath::Normalize(&otherv, mat->quantity());
    comp_ = Composition::CreateFromMass(compmath::Add(v, otherv));
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
  double eps = 1e-3;
  bool decay = false;

  const CompMap c = comp_->atom();
  if (c.size() > 100) {
    decay = true;
  } else {
    CompMap::const_iterator it;
    for (it = c.end(); it != c.begin(); --it) {
      int nuc = it->first;
      // 2419200 == secs / month
      double lambda_months = pyne::decay_const(nuc) * 2419200;

      if (eps <= 1 - std::exp(-lambda_months * dt)) {
        decay = true;
        break;
      }
    }
  }

  if (decay) {
    prev_decay_time_ = curr_time;
    if (dt > 0) {
      Transmute(comp_->Decay(dt));
    }
  }
}

Composition::Ptr Material::comp() const {
  return comp_;
}

Material::Material(Context* ctx, double quantity, Composition::Ptr c)
  : qty_(quantity), comp_(c), tracker_(ctx, this), ctx_(ctx),
    prev_decay_time_(0) {
  if (ctx != NULL) {
    prev_decay_time_ = ctx->time();
  } else {
    tracker_.DontTrack();
  }
}

Material::Ptr NewBlankMaterial(double quantity) {
  Composition::Ptr comp = Composition::CreateFromMass(CompMap());
  return Material::CreateUntracked(quantity, comp);
}

}  // namespace cyclus
