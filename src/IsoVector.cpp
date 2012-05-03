// IsoVector.cpp
#include "IsoVector.h"

#include "CycException.h"
#include "MassTable.h"
#include "DecayHandler.h"
#include "RecipeLogger.h"

#include <vector>
#include <string>

using namespace std;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
IsoVector::IsoVector(CompMap& comp) {
  init(comp);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
IsoVector::IsoVector(CompMap& comp, bool atom) {
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
IsoVector::~IsoVector() {
  if (composition_) {
    composition_.reset();
  }
  if (parent_) {
    parent_.reset();
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
IsoVector& IsoVector::operator= (const IsoVector& rhs) {
  composition_ = rhs.comp();
  decay_time_ = rhs.decay_time();
  ID_ = rhs.ID();
  mass_to_atoms_ = rhs.mass_to_atoms();
  parent_ = rhs.parent();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
IsoVector& IsoVector::operator+= (const IsoVector& rhs) {
  IsoVectorPtr comp = mix(*this,rhs,1.0);
  init(*comp->comp());
  return *this;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
IsoVector& IsoVector::operator-= (const IsoVector& rhs) {
  IsoVectorPtr comp = separate(*this,rhs,1.0);
  init(*comp->comp());
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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
bool IsoVector::operator<(const IsoVector& rhs) const {
  return (ID_ < rhs.ID());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool IsoVector::operator== (const IsoVector& rhs) const {
  return (composition_ == rhs.comp());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool IsoVector::operator!= (const IsoVector& rhs) const {
  return !(*this == rhs);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
CompMapPtr IsoVector::comp() const {
  return composition_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
bool IsoVector::logged() const {
  return (ID_ > 0);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
int IsoVector::ID() const {
  return ID_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
double IsoVector::massFraction(const Iso& tope) const {
  validateIsotopeNumber(tope);
  if (composition_->count(tope) == 0) {
    throw CycIndexException("This composition has no Iso: " + tope);
  }
  return (*composition_)[tope];
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
double IsoVector::atomFraction(const Iso& tope) const {
  validateIsotopeNumber(tope);
  if (composition_->count(tope) == 0) {
    throw CycIndexException("This composition has no Iso: " + tope);
  }
  return (*composition_)[tope] * MT->gramsPerMol(tope) / mass_to_atoms_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
IsoVectorPtr IsoVector::parent() const {
  return parent_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
double IsoVector::decay_time() const {
  return decay_time_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
double IsoVector::mass_to_atoms() const {
  return mass_to_atoms_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
IsoVectorPtr IsoVector::me() {
  return shared_from_this();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
IsoVectorPtr IsoVector::root_comp(IsoVectorPtr comp) {
  IsoVectorPtr child = comp;
  while (child->parent()) {
    child = child->parent();
  }
  return child;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double IsoVector::root_decay_time(IsoVectorPtr comp) {
  IsoVectorPtr child = comp;
  double time = comp->decay_time();
  while (child->parent()) {
    child = child->parent();
    time += child->decay_time();
  }
  return time;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void IsoVector::massify(CompMap& comp) {
  double sum = 0.0;
  for (CompMap::iterator ci = comp.begin(); ci != comp.end(); ci++) {
    ci->second *= MT->gramsPerMol(ci->first);
    sum+= ci->second;
  }
  normalize(comp,sum);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void IsoVector::atomify(CompMap& comp) {
  double sum = 0.0;
  for (CompMap::iterator ci = comp.begin(); ci != comp.end(); ci++) {
    validateEntry(ci->first,ci->second);
    ci->second /= MT->gramsPerMol(ci->first);
    sum += ci->second;
  }
  normalize(comp,sum);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void IsoVector::normalize(CompMap& comp) {
  double sum = 0.0;
  for (CompMap::iterator it = comp.begin(); 
       it != comp.end(); it++) {
    validateEntry(it->first,it->second);
    sum += it->second;
  }
  normalize(comp,sum);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void IsoVector::normalize(CompMap& comp, double sum) {
  if ( sum != 1) { // only normalize if needed
    for (CompMap::iterator it = comp.begin(); 
         it != comp.end(); it++) {
      it->second /= sum;
    }
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
IsoVectorPtr IsoVector::decay(IsoVectorPtr parent, double time) {
  IsoVectorPtr child;
  IsoVectorPtr root = root_comp(parent);
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
IsoVectorPtr mix(const IsoVector& c1, const IsoVector& c2, double ratio) {
  CompMap copy_map(CompMap(*c1.comp())); // copy c1's comp map
  CompMapPtr add_map = c2.comp();
  for (CompMap::iterator it = add_map->begin(); it != add_map->end(); it++) {
    double value = it->second * ratio;
    if (copy_map.count(it->first) == 0) {
      copy_map[it->first] = value;
    }
    else {
      copy_map[it->first] += value;
    }
  }
  return IsoVectorPtr(new IsoVector(copy_map));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
IsoVectorPtr mix(const IsoVectorPtr& p_c1, const IsoVectorPtr& p_c2, double ratio) {
  return mix(*p_c1,*p_c2,ratio);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
IsoVectorPtr separate(const IsoVector& c1, const IsoVector& c2, double efficiency) {
  CompMap copy_map(CompMap(*c1.comp())); // copy c1's comp map
  CompMapPtr remove_map = c2.comp();
  for (CompMap::iterator it = remove_map->begin(); it != remove_map->end(); it++) {
    if (copy_map.count(it->first) != 0) {
      double value = copy_map[it->first];
      double subtraction = efficiency * (*remove_map)[it->first];
      double difference = value - subtraction;
      if (difference > 0) {
        copy_map[it->first] -= difference;
      }
      else {
        copy_map.erase(it->first);
      }
    } // end copy_map.count(it->first) != 0
  } // end for loop
  return IsoVectorPtr(new IsoVector(copy_map));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
IsoVectorPtr separate(const IsoVectorPtr& p_c1, const IsoVectorPtr& p_c2, double efficiency) {
  return separate(*p_c1,*p_c2,efficiency);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
IsoVectorPtr IsoVector::executeDecay(IsoVectorPtr parent, double time) {
  double months_per_year = 12;
  double years = time / months_per_year;
  DecayHandler handler;
  // copy parent comp
  CompMapPtr to_decay = CompMapPtr( new CompMap(*parent->comp()) );
  atomify(*to_decay);
  handler.setComp(to_decay);
  handler.decay(years);
  // construct composition from atom-based comp
  IsoVectorPtr child = IsoVectorPtr(new IsoVector(*handler.comp(),true));
  return child;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void IsoVector::init(CompMap& comp) {
  this->reset();
  composition_ = CompMapPtr(new CompMap(comp)); // copy comp into composition_
  this->checkCompMap();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void IsoVector::reset() {
  ID_ = 0;
  decay_time_ = 0;
  parent_.reset();
  composition_.reset();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void IsoVector::checkCompMap() {
  normalize(*composition_);
  validateIsoVector(composition_);
  mass_to_atoms_ = calculateMassAtomRatio(*composition_);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
double IsoVector::calculateMassAtomRatio(CompMap& comp) {
  double sum = 0.0;
  for (CompMap::iterator ci = comp.begin(); ci != comp.end(); ci++) {
    sum+= ci->second;
  }
  return sum;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void IsoVector::setParent(IsoVectorPtr p) {
  parent_ = p;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void IsoVector::setDecayTime(double time) {
  decay_time_ = time;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
int IsoVector::getAtomicNum(Iso tope) {
  validateIsotopeNumber(tope);
  return tope / 1000; // integer division;
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
int IsoVector::getMassNum(Iso tope) {
  validateIsotopeNumber(tope);
  return tope % 1000;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void IsoVector::validateIsoVector(const CompMapPtr& comp) {
  for (CompMap::iterator comp_iter = comp->begin(); 
       comp_iter != comp->end(); comp_iter++) {
    validateEntry(comp_iter->first,comp_iter->second);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void IsoVector::validateEntry(const Iso& tope, const double& value) {
  validateIsotopeNumber(tope);
  validateValue(value);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void IsoVector::validateIsotopeNumber(const Iso& tope) {
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
void IsoVector::validateValue(const double& value) {
  if (value < 0.0) {
    string err_msg = "IsoVector has negative quantity for an isotope.";
    throw CycRangeException(err_msg);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void IsoVector::print() {
  CLOG(LEV_INFO3) << detail(this->comp());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::string IsoVector::detail(CompMapPtr c) {
  stringstream ss;
  vector<string> entries = compStrings(c);
  for (vector<string>::iterator entry = entries.begin(); 
       entry != entries.end(); entry++) {
    CLOG(LEV_INFO3) << *entry;
  }
  return "";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::vector<std::string> IsoVector::compStrings(CompMapPtr c) {
  stringstream ss;
  vector<string> comp_strings;
  for (CompMap::iterator entry = c->begin(); 
       entry != c->end(); entry++) {
    ss.str("");
    ss << entry->first << ": " << entry->second << " % / kg";
    comp_strings.push_back(ss.str());
  }
  return comp_strings;
}
