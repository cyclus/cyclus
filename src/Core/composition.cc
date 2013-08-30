
#include "composition.h"

#include "comp_math.h"
#include "error.h"
#include "decay_handler.h"
#include "event_manager.h"
#include "mass_table.h"

namespace cyclus {

int Composition::nextId_ = 0;

Composition::Ptr Composition::CreateFromAtom(CompMap v) {
  if (!compmath::ValidIsos(v) || !compmath::AllPositive(v)) {
    throw ValueError("invalid isotope or negative quantity in composition vector");
  }

  Composition::Ptr c(new Composition());
  c->atomv_ = v;
  return c;
}

Composition::Ptr Composition::CreateFromMass(CompMap v) {
  if (!compmath::ValidIsos(v) || !compmath::AllPositive(v)) {
    throw ValueError("invalid isotope or negative quantity in composition vector");
  }

  Composition::Ptr c(new Composition());
  c->massv_ = v;
  return c;
}

int Composition::id() {
  return id_;
}

const CompMap& Composition::atom_vect() {
  if (atomv_.size() == 0) {
    CompMap::iterator it;
    for (it = massv_.begin(); it != massv_.end(); ++it) {
      Iso iso = it->first;
      atomv_[iso] = massv_[iso] / MT->GramsPerMol(iso);
    }
  }
  return atomv_;
}

const CompMap& Composition::mass_vect() {
  if (massv_.size() == 0) {
    CompMap::iterator it;
    for (it = atomv_.begin(); it != atomv_.end(); ++it) {
      Iso iso = it->first;
      massv_[iso] = atomv_[iso] * MT->GramsPerMol(iso);
    }
  }
  return massv_;
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
    for (it = mass_vect().begin(); it != mass_vect().end(); ++it) {
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
  id_ = nextId_;
  nextId_++;
  decay_line_ = ChainPtr(new Chain());
}

Composition::Composition(int prev_decay, ChainPtr decay_line)
    : recorded_(false),
      prev_decay_(prev_decay),
      decay_line_(decay_line) {
  id_ = nextId_;
  nextId_++;
}

Composition::Ptr Composition::NewDecay(int delta) {
  int tot_decay = prev_decay_ + delta;
  double months_per_year = 12;
  double years = double(delta) / months_per_year;

  DecayHandler handler(atomv_);
  handler.decay(years);

  Composition::Ptr decayed(new Composition(tot_decay, decay_line_));
  handler.getResult(decayed->atomv_);
	(*decay_line_)[tot_decay] = decayed;

  return decayed;
}

} // namespace cyclus
