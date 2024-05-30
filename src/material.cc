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
                               Composition::Ptr c, std::string package_name) {
  Material::Ptr m(new Material(creator->context(), quantity, c, package_name));
  m->tracker_.Create(creator);
  return m;
}

Material::Ptr Material::CreateUntracked(double quantity,
                                        Composition::Ptr c) {
  Material::Ptr m(new Material(NULL, quantity, c, Package::unpackaged_name()));
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

  // TODO: decide if ExtractComp should force lazy-decay by calling comp()
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
  Material::Ptr other(new Material(ctx_, qty, c, Package::unpackaged_name()));

  // Decay called on the extracted material should have the same dt as for
  // this material regardless of composition.
  other->prev_decay_time_ = prev_decay_time_;

  tracker_.Extract(&other->tracker_);

  return other;
}

void Material::Absorb(Material::Ptr mat) {
  // these calls force lazy evaluation if in lazy decay mode
  Composition::Ptr c0 = comp();
  Composition::Ptr c1 = mat->comp();

  if (c0 != c1) {
    CompMap v(c0->mass());
    compmath::Normalize(&v, qty_);
    CompMap otherv(c1->mass());
    compmath::Normalize(&otherv, mat->qty_);
    comp_ = Composition::CreateFromMass(compmath::Add(v, otherv));
  }

  // Set the decay time to the value of the material that had the larger
  // quantity.  This helps avoid inheriting erroneous prev decay times if, for
  // example, you absorb a material into a zero-quantity material that had a
  // prev decay time prior to the current simulation time step.
  if (qty_ < mat->qty_) {
    prev_decay_time_ = mat->prev_decay_time_;
  }

  qty_ += mat->qty_;
  mat->qty_ = 0;
  tracker_.Absorb(&mat->tracker_);
}

void Material::Transmute(Composition::Ptr c) {
  comp_ = c;
  tracker_.Modify();

  // Presumably the user has chosen the new composition to be accurate for
  // the current simulation time.  The next call to decay should not include
  // accumulated decay delta t from a composition that no longer exists in
  // this material.  This ---------+ condition allows testing to work.
  //                               |
  //                               |
  //                               V
  if (ctx_ != NULL && ctx_->time() > prev_decay_time_) {
    prev_decay_time_ = ctx_->time();
  }
}

void Material::ChangePackage(std::string new_package_name) {
  if (new_package_name == package_name_ || ctx_ == NULL) {
    // no change needed
    return;
  }
  else if (new_package_name == Package::unpackaged_name()) {
    // unpackaged has functionally no restrictions
    package_name_ = new_package_name;
    tracker_.Package();
    return;
  }
 
  Package::Ptr p = ctx_->GetPackage(package_name_);
  double min = p->fill_min();
  double max = p->fill_max();
  if (qty_ >= min && qty_ <= max) {
    package_name_ = new_package_name;
    tracker_.Package();
  } else {
    throw ValueError("Material quantity is outside of package fill limits.");
  }
}

void Material::Decay(int curr_time) {
  if (ctx_ != NULL && ctx_->sim_info().decay == "never") {
    return;
  } else if (curr_time < 0 && ctx_ == NULL) {
    throw ValueError("decay cannot use default time with NULL context");
  }

  if (curr_time < 0) {
    curr_time = ctx_->time();
  }

  int dt = curr_time - prev_decay_time_;
  if (dt == 0) {
    return;
  }

  double eps = 1e-3;
  const CompMap c = comp_->atom();

  // If composition has too many nuclides (i.e. > 100), it is cheaper to
  // just do the decay rather than check all the decay constants.
  bool decay = c.size() > 100;

  uint64_t secs_per_timestep = kDefaultTimeStepDur;
  if (ctx_ != NULL) {
    secs_per_timestep = ctx_->sim_info().dt;
  }

  if (!decay) {
    // Only do the decay calc if one of the nuclides would change in number
    // density more than fraction eps.
    // i.e. decay if   (1 - eps) > exp(-lambda*dt)
    CompMap::const_reverse_iterator it;
    for (it = c.rbegin(); it != c.rend(); ++it) {
      int nuc = it->first;
      double lambda_timesteps = pyne::decay_const(nuc) * static_cast<double>(secs_per_timestep);
      double change = 1.0 - std::exp(-lambda_timesteps * static_cast<double>(dt));
      if (change >= eps) {
        decay = true;
        break;
      }
    }
    if (!decay) {
      return;
    }
  }

  prev_decay_time_ = curr_time; // this must go before Transmute call
  Composition::Ptr decayed = comp_->Decay(dt, secs_per_timestep);
  Transmute(decayed);
}

double Material::DecayHeat() {
  double decay_heat = 0.;
  // Pyne decay heat operates with grams, cyclus generally in kilograms.
  pyne::Material p_map = pyne::Material(comp_->mass(), qty_ * 1000);
  std::map<int, double> dec_heat = p_map.decay_heat();
  for (auto nuc : dec_heat) {
    if (!std::isnan(nuc.second)) {
      decay_heat += nuc.second;
    }
  }
  return decay_heat;
}

Composition::Ptr Material::comp() const {
  throw Error("comp() const is deprecated - use non-const comp() function."
              " Recompilation should fix the problem.");
}

Composition::Ptr Material::comp() {
  if (ctx_ != NULL && ctx_->sim_info().decay == "lazy") {
    Decay(-1);
  }
  return comp_;
}

Material::Material(Context* ctx, double quantity, Composition::Ptr c, std::string package_name)
    : qty_(quantity),
      comp_(c),
      tracker_(ctx, this),
      ctx_(ctx),
      prev_decay_time_(0),
      package_name_(package_name) {
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

std::string Material::package_name() {
  return package_name_;
}

}  // namespace cyclus
