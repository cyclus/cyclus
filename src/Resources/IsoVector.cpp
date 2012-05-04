// IsoVector.cpp
#include "IsoVector.h"

#include "CycException.h"
#include "MassTable.h"
#include "Logger.h"
#include "DecayHandler.h"
#include "RecipeLogger.h"

#include <vector>
#include <string>
#include <sstream>
#include <algorithm> // std::swap

using namespace std;

LogLevel IsoVector::log_level_ = LEV_INFO3;

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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void swap(IsoVector& v1, IsoVector& v2) {
  using std::swap;
  swap(v1.composition_,v2.composition_);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
IsoVector::IsoVector(const IsoVector& other) {
  swap( *this, const_cast<IsoVector&>(other) );
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
IsoVector& IsoVector::operator= (IsoVector rhs) {
  swap(*this,rhs);
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
double IsoVector::massFraction(const Iso& tope) const {
  return composition_->massFraction(tope);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
double IsoVector::atomFraction(const Iso& tope) const {
  return composition_->atomFraction(tope);
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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void IsoVector::mix(const IsoVector& other, double ratio) {
  CompMapPtr new_comp = 
    CompMapPtr( new CompMap(composition_->basis(),composition_->map()) );
  CompMapPtr add_comp = other.comp();
  for (CompMap::iterator it = add_comp->begin(); it 
         != add_comp->end(); it++) {
    // check ratio
    if (ratio < 0) {
      stringstream ss("");
      ss << "Ratio: " << ratio << " is not in [0,inf).";
      throw CycRangeException(ss.str());
    }
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
  new_comp->normalize();
  setComp(new_comp);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void IsoVector::mix(const IsoVectorPtr& p_other, double ratio) {
  mix(*p_other,ratio);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void IsoVector::separate(const IsoVector& other, double efficiency) {  
  CompMapPtr new_comp = 
    CompMapPtr( new CompMap(composition_->basis(),composition_->map()) );
  CompMapPtr remove_comp = other.comp();
  for (CompMap::iterator it = remove_comp->begin(); 
       it != remove_comp->end(); it++) {
    // reduce isotope, if it exists in new_comp
    if (new_comp->count(it->first) != 0) {
      if (efficiency > 1.0 || efficiency < 0) {
        stringstream ss("");
        ss << "Efficiency: " << efficiency << " is not in [0,1].";
        throw CycRangeException(ss.str());
      }
      else if (efficiency < 1.0) {
        (*new_comp)[it->first] -= efficiency * (*new_comp)[it->first];
      }
      else {
        new_comp->erase(it->first);
      }
    }
  } 
  new_comp->normalize();
  setComp(new_comp);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void IsoVector::separate(const IsoVectorPtr& p_other, double efficiency) {
  separate(*p_other,efficiency);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void IsoVector::decay(double time) {
  CompMapPtr child;
  CompMapPtr parent = composition_;
  CompMapPtr root = parent->root_comp();
  bool root_logged = root->logged();
  if (root_logged) { 
    int t_f = composition_->root_decay_time() + time;
    bool child_logged = RL->daughterLogged(parent,t_f);
    if (child_logged) {
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
  child->normalize();
  setComp(child);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void IsoVector::setComp(CompMapPtr comp) {
  composition_ = comp;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
CompMapPtr IsoVector::executeDecay(CompMapPtr parent, double time) {
  double months_per_year = 12;
  double years = time / months_per_year;
  DecayHandler handler;
  parent->set_basis(ATOM);
  handler.setComp(parent); // handler will not change parent's map
  handler.decay(years);
  return handler.comp();
}
