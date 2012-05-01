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
bool Composition::operator<(Composition& other) {
  return (ID_ < other.ID());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
bool Composition::logged() {
  return (ID > 0);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
CompositionPtr Composition::parent() {
  if (!parent_) {
    throw 
      CycIndexException("parent pointer to composition not initialized.");
  }
  return parent_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void Composition::massify(CompMap& comp) {
  for (CompMap::iterator ci = comp->begin(); ci != comp->end(); ci++) {
    ci->second *= MT->gramsPerMol(ci->first);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void Composition::normalize(CompMap& comp) {
  double total = 0.0;
  for (CompMap::iterator it = comp->begin(); 
       it != comp->end(); it++) {
    total += it->second;
    validateEntry(it->first,it->second);
  }
  if (total != 1) { // only normalize if needed
    for (CompMap::iterator it = comp->begin(); 
         it != comp->end(); it++) {
      it->second /= total;
    }
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
Composition::init(CompMap& comp) {
  normalize(comp);
  composition_ = CompMapPtr(new CompMap(comp));
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
  parent_.reset(p);
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
void Composition::validateEntry(Iso tope, double value) {
  validateIsotopeNumber(tope);
  validateValue(value);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void Composition::validateIsotopeNumber(Iso tope) {
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
void Composition::validateValue(double value) {
  if (value < 0.0) {
    string err_msg = "Composition has negative quantity for an isotope.";
    throw CycRangeException(err_msg);
  }
}
