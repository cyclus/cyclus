// IsoVector.cpp
#include "IsoVector.h"

#include "CycException.h"
#include "Logger.h"
#include "DecayHandler.h"
#include "RecipeLibrary.h"

#include <vector>
#include <string>
#include <sstream>

using namespace std;

LogLevel IsoVector::log_level_ = LEV_INFO3;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
IsoVector::IsoVector() {
  CompMapPtr comp = CompMapPtr(new CompMap(MASS));
  setComp(comp);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
IsoVector::IsoVector(CompMapPtr comp) {
  setComp(comp);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
IsoVector::~IsoVector() {
  if (composition_) {
    composition_.reset();
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
IsoVector::IsoVector(const IsoVector& other) {
  setComp(other.comp());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
IsoVector& IsoVector::operator= (IsoVector rhs) {
  setComp(rhs.comp());
  return *this;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
IsoVector& IsoVector::operator+= (const IsoVector& rhs) {
  this->mix(rhs,1.0);
  return *this;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
IsoVector& IsoVector::operator-= (const IsoVector& rhs) {
  this->separate(rhs,1.0);
  return *this;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const IsoVector IsoVector::operator+ (const IsoVector& rhs) const {
  IsoVector result = *this;
  result += rhs;
  return result;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const IsoVector IsoVector::operator- (const IsoVector& rhs) const {
  IsoVector result = *this;
  result -= rhs;
  return result;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool IsoVector::operator== (const IsoVector& rhs) const {
  return (composition_ == rhs.comp());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool IsoVector::operator!= (const IsoVector& rhs) const {
  return ! (*this == rhs);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
CompMapPtr IsoVector::comp() const {
  return composition_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
double IsoVector::massFraction(Iso tope) {
  return composition_->massFraction(tope);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
double IsoVector::atomFraction(Iso tope) {
  return composition_->atomFraction(tope);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void IsoVector::normalize() {
  composition_->normalize();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void IsoVector::validate() {
  composition_->validate();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void IsoVector::reset() {
  composition_.reset();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void IsoVector::print() {
  CLOG(log_level_) << "This IsoVector manages: ";
  composition_->print();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void IsoVector::log() {
  RL->logRecipe(composition_);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double IsoVector::intersectionFraction(const IsoVector& other) {
  double fraction = 0;
  CompMapPtr other_comp = other.comp();
  for (CompMap::iterator it = other_comp->begin(); it 
         != other_comp->end(); it++) {
    if (composition_->count(it->first) > 0) {
      fraction += massFraction(it->first);
    }
  }
  return fraction;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool IsoVector::compEquals(const IsoVector& other) {
  return (compEquals(*other.comp()));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool IsoVector::compEquals(const CompMap& comp) {
  return (*composition_ == comp);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void IsoVector::mix(const IsoVector& other, double ratio) {
  if (ratio < 0) { // check ratio
    stringstream ss("");
    ss << "Ratio: " << ratio << " is not in [0,inf).";
    throw CycRangeException(ss.str());
  }
  // get base comp and comp to add
  CompMapPtr new_comp = CompMapPtr(new CompMap(*composition_)); // copy
  CompMapPtr add_comp = other.comp();
  // loop over comp to add
  for (CompMap::iterator it = add_comp->begin(); it 
         != add_comp->end(); it++) {
    // get correct value to add
    double value;
    if ( new_comp->basis() == add_comp->basis() ) {
      value = it->second;
    }
    else if (add_comp->basis() == MASS){ 
      value = add_comp->massFraction(it->first);
    }
    else {
      value = add_comp->atomFraction(it->first);
    }
    value *= ratio; // adjust value
    // add it
    if (new_comp->count(it->first) != 0) {
      (*new_comp)[it->first] += value;
    }
    else {
      (*new_comp)[it->first] = value;
    }
  }
  setComp(new_comp);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void IsoVector::mix(const IsoVectorPtr& p_other, double ratio) {
  mix(*p_other,ratio);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void IsoVector::separate(const IsoVector& other, double efficiency) {  
  if (efficiency > 1.0 || efficiency < 0) {  // check efficiency
    stringstream ss("");
    ss << "Efficiency: " << efficiency << " is not in [0,1].";
    throw CycRangeException(ss.str());
  }
  CompMapPtr new_comp = CompMapPtr(new CompMap(*composition_));
  CompMapPtr remove_comp = other.comp();
  for (CompMap::iterator it = remove_comp->begin(); 
       it != remove_comp->end(); it++) {
    // reduce isotope, if it exists in new_comp
    if (new_comp->count(it->first) != 0) {
      if (efficiency != 1.0) {
        (*new_comp)[it->first] -= efficiency * (*new_comp)[it->first];
      }
      else {
        new_comp->erase(it->first);
      }
    }
  }
  setComp(new_comp);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void IsoVector::separate(const IsoVectorPtr& p_other, double efficiency) {
  separate(*p_other,efficiency);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void IsoVector::decay(double time) {
  CompMapPtr parent = composition_;
  CompMapPtr root = parent->root_comp();
  bool root_recorded = root->recorded();
  Basis orig_basis = parent->basis();
  CompMapPtr child;
  if (root_recorded) { 
    int t_f = parent->root_decay_time() + time;
    bool child_recorded = RL->daughterRecorded(parent,t_f);
    if (child_recorded) {
      child = RL->Daughter(parent,t_f);
    }
    else {
      child = executeDecay(parent,time); // do decay and log it
      RL->logRecipeDecay(parent,child,t_f);
    }
  }
  else {
    child = executeDecay(parent,time); // just do decay
  }
  child->change_basis(orig_basis);
  setComp(child);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void IsoVector::setComp(CompMapPtr comp) {
  if (!comp->normalized()) {
    comp->normalize();
  }
  composition_ = comp;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
CompMapPtr IsoVector::executeDecay(CompMapPtr parent, double time) {
  double months_per_year = 12;
  double years = time / months_per_year;
  DecayHandler handler;
  parent->atomify();
  handler.setComp(parent); // handler will not change parent's map
  handler.decay(years);
  CompMapPtr child = handler.comp();
  child->parent_ = parent;
  child->decay_time_ = time;
  return child;
}
