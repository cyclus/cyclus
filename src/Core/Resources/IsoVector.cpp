// IsoVector.cpp
#include "IsoVector.h"

#include "error.h"
#include "Logger.h"
#include "DecayHandler.h"
#include "RecipeLibrary.h"

#include <vector>
#include <string>
#include <sstream>

namespace cyclus {

LogLevel IsoVector::record_level_ = LEV_INFO3;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
IsoVector::IsoVector() {
  CompMapPtr comp = CompMapPtr(new CompMap(MASS));
  SetComp(comp);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
IsoVector::IsoVector(CompMapPtr comp) {
  SetComp(comp);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
IsoVector::~IsoVector() {
  if (composition_) {
    composition_.reset();
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
IsoVector::IsoVector(const IsoVector& other) {
  SetComp(other.comp());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
IsoVector& IsoVector::operator= (IsoVector rhs) {
  SetComp(rhs.comp());
  return *this;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
IsoVector& IsoVector::operator+= (const IsoVector& rhs) {
  this->Mix(rhs,1.0);
  return *this;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
IsoVector& IsoVector::operator-= (const IsoVector& rhs) {
  this->Separate(rhs,1.0);
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
  return CompEquals(rhs);
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
double IsoVector::MassFraction(Iso tope) {
  return composition_->MassFraction(tope);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
double IsoVector::AtomFraction(Iso tope) {
  return composition_->AtomFraction(tope);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void IsoVector::normalize() {
  composition_->normalize();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void IsoVector::Validate() {
  composition_->Validate();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void IsoVector::reset() {
  composition_.reset();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void IsoVector::Print() {
  CLOG(record_level_) << "This IsoVector manages: ";
  composition_->Print();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void IsoVector::Record() {
  RL->RecordRecipe(composition_);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double IsoVector::IntersectionFraction(const IsoVector& other) {
  double fraction = 0;
  CompMapPtr other_comp = other.comp();
  for (CompMap::iterator it = other_comp->begin(); it 
         != other_comp->end(); it++) {
    if (composition_->count(it->first) > 0) {
      fraction += MassFraction(it->first);
    }
  }
  return fraction;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool IsoVector::CompEquals(const IsoVector& other) const {
  return (CompEquals(other.comp()));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool IsoVector::CompEquals(const CompMapPtr comp) const {
  return (*composition_ == *comp);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void IsoVector::Mix(const IsoVector& other, double ratio) {
  if (ratio < 0) { // check ratio
    std::stringstream ss("");
    ss << "Ratio: " << ratio << " is not in [0,inf).";
    throw ValueError(ss.str());
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
      value = add_comp->MassFraction(it->first);
    }
    else {
      value = add_comp->AtomFraction(it->first);
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
  SetComp(new_comp);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void IsoVector::Mix(const IsoVectorPtr& p_other, double ratio) {
  Mix(*p_other,ratio);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void IsoVector::Separate(const IsoVector& other, double efficiency) {  
  if (efficiency > 1.0 || efficiency < 0) {  // check efficiency
    std::stringstream ss("");
    ss << "Efficiency: " << efficiency << " is not in [0,1].";
    throw ValueError(ss.str());
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
  SetComp(new_comp);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void IsoVector::Separate(const IsoVectorPtr& p_other, double efficiency) {
  Separate(*p_other,efficiency);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void IsoVector::Decay(int time) {
  CompMapPtr parent = composition_;
  CompMapPtr root = parent->root_comp();
  CompMapPtr child;

  if (root->Recorded()) { 
    int t_f = parent->root_decay_time() + time;
    if (RL->ChildRecorded(parent, t_f)) {
      child = RL->Child(parent, t_f);
    }
    else {
      child = ExecuteDecay(parent, time); // do decay and record it
      RL->RecordRecipeDecay(parent, child, t_f);
    }
  }
  else {
    child = ExecuteDecay(parent, time); // just do decay
  }
  child->Change_basis(parent->basis());
  SetComp(child);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void IsoVector::SetComp(CompMapPtr comp) {
  if (!comp->normalized()) {
    comp->normalize();
  }
  composition_ = comp;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
CompMapPtr IsoVector::ExecuteDecay(CompMapPtr parent, int time) {
  double months_per_year = 12;
  double years = double(time) / months_per_year;
  DecayHandler handler;
  parent->Atomify();
  handler.SetComp(parent); // handler will not change parent's map
  handler.Decay(years);
  CompMapPtr child = handler.comp();
  child->parent_ = parent;
  child->decay_time_ = time;
  return child;
}
} // namespace cyclus
