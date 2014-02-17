
#include "composition.h"

#include "comp_math.h"
#include "context.h"
#include "error.h"
#include "decayer.h"
#include "recorder.h"
#include "mass_table.h"

namespace cyclus {

int Composition::next_id_ = 0;

Composition::Ptr Composition::CreateFromAtom(CompMap v) {
  if (!compmath::ValidNucs(v) || !compmath::AllPositive(v)) {
    throw ValueError("invalid nuclide or negative quantity in CompMap");
  }

  Composition::Ptr c(new Composition());
  c->atom_ = v;
  return c;
}

Composition::Ptr Composition::CreateFromMass(CompMap v) {
  if (!compmath::ValidNucs(v) || !compmath::AllPositive(v)) {
    throw ValueError("invalid nuclide or negative quantity in CompMap");
  }

  Composition::Ptr c(new Composition());
  c->mass_ = v;
  return c;
}

int Composition::id() {
  return id_;
}

const CompMap& Composition::atom() {
  if (atom_.size() == 0) {
    CompMap::iterator it;
    for (it = mass_.begin(); it != mass_.end(); ++it) {
      Nuc iso = it->first;
      atom_[iso] = mass_[iso] / MT->GramsPerMol(iso);
    }
  }
  return atom_;
}

const CompMap& Composition::mass() {
  if (mass_.size() == 0) {
    CompMap::iterator it;
    for (it = atom_.begin(); it != atom_.end(); ++it) {
      Nuc iso = it->first;
      mass_[iso] = atom_[iso] * MT->GramsPerMol(iso);
    }
  }
  return mass_;
}

Composition::Ptr Composition::Decay(int delta) {
  int tot_decay = prev_decay_ + delta;
  if (decay_line_->count(tot_decay) == 1) {
    // decay_line_ already has a comp decayed tot_decay.
    return (*decay_line_)[tot_decay];
  }

  // Calculate a new decayed composition and insert it into the decay chain.
  // It will automagically appear in the decay chain for all other compositions
  // that are a part of this decay chain because decay_line_ is a pointer that
  // all compositions in the chain share.
  Composition::Ptr decayed = NewDecay(delta);
  (*decay_line_)[tot_decay] = decayed;
  return decayed;
}

void Composition::Record(Context* ctx) {
  if (!recorded_) {
    CompMap::const_iterator it;
    mass(); // force lazy evaluation now
    compmath::Normalize(&mass_, 1);
    for (it = mass().begin(); it != mass().end(); ++it) {
      ctx->NewDatum("Compositions")
         ->AddVal("ID", id())
         ->AddVal("NucID", it->first)
         ->AddVal("Quantity", it->second)
         ->Record();
    }
    recorded_ = true;
  }
}

Composition::Composition()
  : prev_decay_(0), recorded_(false) {
  id_ = next_id_;
  next_id_++;
  decay_line_ = ChainPtr(new Chain());
}

Composition::Composition(int prev_decay, ChainPtr decay_line)
  : recorded_(false),
    prev_decay_(prev_decay),
    decay_line_(decay_line) {
  id_ = next_id_;
  next_id_++;
}

Composition::Ptr Composition::NewDecay(int delta) {
  int tot_decay = prev_decay_ + delta;
  double months_per_year = 12;
  double years = double(delta) / months_per_year;

  Decayer handler(atom_);
  handler.Decay(years);

  // the new composition is a part of this decay chain and so is created with a
  // pointer to the exact same decay_line_.
  Composition::Ptr decayed(new Composition(tot_decay, decay_line_));
  handler.GetResult(decayed->atom_);
  return decayed;
}

} // namespace cyclus
