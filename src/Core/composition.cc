
#include "composition.h"

#include "comp_math.h"
#include "error.h"
#include "decay_handler.h"
#include "event_manager.h"
#include "mass_table.h"

namespace cyclus {

int Composition::next_id_ = 0;

Composition::Ptr Composition::CreateFromAtom(CompMap v) {
  if (!compmath::ValidIsos(v) || !compmath::AllPositive(v)) {
    throw ValueError("invalid isotope or negative quantity in CompMap");
  }

  Composition::Ptr c(new Composition());
  c->atom_ = v;
  return c;
}

Composition::Ptr Composition::CreateFromMass(CompMap v) {
  if (!compmath::ValidIsos(v) || !compmath::AllPositive(v)) {
    throw ValueError("invalid isotope or negative quantity in CompMap");
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
      Iso iso = it->first;
      atom_[iso] = mass_[iso] / MT->GramsPerMol(iso);
    }
  }
  return atom_;
}

const CompMap& Composition::mass() {
  if (mass_.size() == 0) {
    CompMap::iterator it;
    for (it = atom_.begin(); it != atom_.end(); ++it) {
      Iso iso = it->first;
      mass_[iso] = atom_[iso] * MT->GramsPerMol(iso);
    }
  }
  return mass_;
}

Composition::Ptr Composition::Decay(int delta) {
  int tot_decay = prev_decay_ + delta;
  if (decay_line_->count(tot_decay) == 1) {
    return (*decay_line_)[tot_decay];
  }
  return NewDecay(delta);
}

void Composition::Record() {
  if (!recorded_) {
    CompMap::const_iterator it;
    for (it = mass().begin(); it != mass().end(); ++it) {
      EM->NewEvent("Compositions")
      ->AddVal("ID", id())
      ->AddVal("IsoID", it->first)
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

  DecayHandler handler(atom_);
  handler.decay(years);

  Composition::Ptr decayed(new Composition(tot_decay, decay_line_));
  handler.getResult(decayed->atom_);
  (*decay_line_)[tot_decay] = decayed;

  return decayed;
}

} // namespace cyclus
