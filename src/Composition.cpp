// Composition.cpp
#include "Composition.h"

#include "CycException.h"
#include "MassTable.h"

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
CompositionPtr Composition::parent() const {
  if (!parent_) {
    throw 
      CycIndexException("parent pointer to composition not initialized.");
  }
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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void Composition::massify(CompMap& comp) {
  double sum;
  for (CompMap::iterator ci = comp.begin(); ci != comp.end(); ci++) {
    ci->second *= MT->gramsPerMol(ci->first);
    sum+= ci->second;
  }
  normalize(comp,sum);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void Composition::atomify(CompMap& comp) {
  double sum;
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
void Composition::init(CompMap& comp) {
  normalize(comp);
  composition_ = CompMapPtr(new CompMap(comp)); // copy comp into composition_
  validateComposition(composition_);
  ID_ = 0;
  decay_time_ = 0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void Composition::setParent(Composition* p) {
  this->setParent(CompositionPtr(p));
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
