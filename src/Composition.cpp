// Composition.cpp
#include "Composition.h"

#include "CycException.h"
#include "MassTable.h"
#include "DecayHandler.h"

using namespace std;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
Composition::Composition(CompMap& comp) {
  init(comp);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
Composition::Composition(CompMap& comp, bool atom) {
  if (atom) {
    CompMap comp_copy = CompMap(comp);
    massify(comp_copy);
    init(comp_copy);
  }
  else {
    init(comp);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
Composition::~Composition() {
  if (composition_) {
    composition_.reset();
  }
  if (parent_) {
    parent_.reset();
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
bool Composition::operator<(const Composition& other) const {
  return (ID_ < other.ID());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
bool Composition::logged() const {
  return (ID_ > 0);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
int Composition::ID() const {
  return ID_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
double Composition::massFraction(const Iso& tope) const {
  validateIsotopeNumber(tope);
  if (composition->count(tope) == 0) {
    throw CycIndexException("This composition has no Iso: " + tope);
  }
  return (*composition_)[tope];
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
double Composition::atomFraction(const Iso& tope) const {
  validateIsotopeNumber(tope);
  if (composition->count(tope) == 0) {
    throw CycIndexException("This composition has no Iso: " + tope);
  }
  return (*composition_)[tope] * MT->gramsPerMol(tope) / mass_to_atoms_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
CompositionPtr Composition::parent() const {
  return parent_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
double Composition::decay_time() const {
  return decay_time_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
CompositionPtr Composition::me() {
  return shared_from_this();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CompositionPtr Composition::root_comp(CompositionPtr comp) {
  CompositionPtr child = comp;
  while (child->parent()) {
    child = child->parent();
  }
  return child;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double Composition::root_decay_time(CompositionPtr comp) {
  CompositionPtr child = comp;
  double time = comp->decay_time();
  while (child->parent()) {
    child = child->parent();
    time += child->decay_time();
  }
  return time;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void Composition::massify(CompMap& comp) {
  double sum = 0.0;
  for (CompMap::iterator ci = comp.begin(); ci != comp.end(); ci++) {
    ci->second *= MT->gramsPerMol(ci->first);
    sum+= ci->second;
  }
  normalize(comp,sum);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void Composition::atomify(CompMap& comp) {
  double sum = 0.0;
  for (CompMap::iterator ci = comp.begin(); ci != comp.end(); ci++) {
    validateEntry(ci->first,ci->second);
    ci->second /= MT->gramsPerMol(ci->first);
    sum += ci->second;
  }
  normalize(comp,sum);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void Composition::normalize(CompMap& comp) {
  double sum = 0.0;
  for (CompMap::iterator it = comp.begin(); 
       it != comp.end(); it++) {
    validateEntry(it->first,it->second);
    sum += it->second;
  }
  normalize(comp,sum);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void Composition::normalize(CompMap& comp, double sum) {
  if ( sum != 1) { // only normalize if needed
    for (CompMap::iterator it = comp.begin(); 
         it != comp.end(); it++) {
      it->second /= sum;
    }
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
CompositionPtr Composition::decay(CompositionPtr parent, double time) {
  CompositionPtr child;
  CompositionPtr root = root_comp(parent);
  if (root->logged()) { 
    int t_f = root_decay_time(parent) + time;
    bool logged = RL->daughterLogged(root,t_f);
    if (logged) {
      child = RL->Daughter(root,t_f);
    } // end logged
    else {
      child = executeDecay(parent,time);
      RL->logRecipeDecay(parent,child,t_f);
    }
  } // end root->logged()
  else {
    child = executeDecay(parent,time);
  }
  return child;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
CompositionPtr Composition::executeDecay(CompositionPtr parent, double time) {
  double months_per_year = 12;
  double years = time / months_per_year;

  // perform decay
  DecayHandler handler;
  CompMapPtr child = CompMapPtr(new CompMap(*parent->comp())); // copy parent comp
  atomify(*child);
  handler.setComp(child);
  handler.decay(years);
  child.reset(handler.comp());
  massify(*child);
  return child;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void Composition::init(CompMap& comp) {
  normalize(comp);
  composition_ = CompMapPtr(new CompMap(comp)); // copy comp into composition_
  validateComposition(composition_);
  ID_ = 0;
  decay_time_ = 0;
  mass_to_atoms_ = calculateMassAtomRatio(comp);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
double Composition::calculateMassAtomRatio(CompMap& comp) {
  double sum = 0.0;
  for (CompMap::iterator ci = comp.begin(); ci != comp.end(); ci++) {
    sum+= ci->second;
  }
  return sum;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void Composition::setParent(CompositionPtr p) {
  parent_ = p;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void Composition::setDecayTime(int time) {
  decay_time_ = time;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
int Composition::getAtomicNum(Iso tope) {
  validateIsotopeNumber(tope);
  return tope / 1000; // integer division;
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
int Composition::getMassNum(Iso tope) {
  validateIsotopeNumber(tope);
  return tope % 1000;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void Composition::validateComposition(const CompMapPtr& comp) {
  for (CompMap::iterator comp_iter = comp->begin(); 
       comp_iter != comp->end(); comp_iter++) {
    validateEntry(comp_iter->first,comp_iter->second);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void Composition::validateEntry(const Iso& tope, const double& value) {
  validateIsotopeNumber(tope);
  validateValue(value);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void Composition::validateIsotopeNumber(const Iso& tope) {
  int lower_limit = 1001;
  int upper_limit = 1182949;
  
  if (tope < lower_limit || tope > upper_limit) {
    stringstream st;
    st << tope;
    string isotope = st.str();
    throw CycRangeException("Isotope identifier '" + isotope + "' is not valid.");
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void Composition::validateValue(const double& value) {
  if (value < 0.0) {
    string err_msg = "Composition has negative quantity for an isotope.";
    throw CycRangeException(err_msg);
  }
}
