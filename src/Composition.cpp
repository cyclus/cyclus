// Composition.cpp
#include "Composition.h"

#include "CycException.h"
#include "MassTable.h"
#include "DecayHandler.h"
#include "RecipeLogger.h"

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
CompMapPtr Composition::comp() const {
  return composition_;
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
  if (composition_->count(tope) == 0) {
    throw CycIndexException("This composition has no Iso: " + tope);
  }
  return (*composition_)[tope];
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
double Composition::atomFraction(const Iso& tope) const {
  validateIsotopeNumber(tope);
  if (composition_->count(tope) == 0) {
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
CompositionPtr mix(const Composition& c1, const Composition& c2, double ratio) {
  CompMap copy_map(CompMap(*c1.comp())); // copy c1's comp map
  CompMapPtr add_map = c2.comp();
  for (CompMap::iterator it = add_map->begin(); it != add_map->end(); it++) {
    if (copy_map.count(it->first) == 0) {
      copy_map[it->first] = it->second;
    }
    else {
      copy_map[it->first] += it->second;
    }
  }
  return CompositionPtr(new Composition(copy_map));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
CompositionPtr mix(const CompositionPtr& p_c1, const CompositionPtr& p_c2, double ratio) {
  return mix(*p_c1,*p_c2,ratio);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
CompositionPtr separate(const Composition& c1, const Composition& c2, double efficiency) {
  CompMap copy_map(CompMap(*c1.comp())); // copy c1's comp map
  CompMapPtr remove_map = c2.comp();
  for (CompMap::iterator it = remove_map->begin(); it != remove_map->end(); it++) {
    if (copy_map.count(it->first) != 0) {
      double value = copy_map[it->first];
      double subtraction = efficiency * (*remove_map)[it->first];
      double difference = value- subtraction;
      if (difference > 0) {
        copy_map[it->first] -= difference;
      }
      else {
        copy_map.erase(it->first);
      }
    } // end copy_map.count(it->first) != 0
  } // end for loop
  return CompositionPtr(new Composition(copy_map));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
CompositionPtr separate(const CompositionPtr& p_c1, const CompositionPtr& p_c2, double efficiency) {
  return separate(*p_c1,*p_c2,efficiency);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
CompositionPtr Composition::executeDecay(CompositionPtr parent, double time) {
  double months_per_year = 12;
  double years = time / months_per_year;
  DecayHandler handler;
  // copy parent comp
  CompMapPtr to_decay = CompMapPtr( new CompMap(*parent->comp()) );
  atomify(*to_decay);
  handler.setComp(to_decay);
  handler.decay(years);
  // construct composition from atom-based comp
  CompositionPtr child = CompositionPtr(new Composition(*handler.comp(),true));
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
void Composition::setDecayTime(double time) {
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
