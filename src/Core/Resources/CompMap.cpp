// CompMap.cpp
#include "CompMap.h"

#include "MassTable.h"
#include "CycException.h"

#include <sstream>
#include <cmath> // std::abs

using namespace std;

LogLevel CompMap::log_level_ = LEV_INFO3;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
CompMap::CompMap(Basis b) {
  init(b);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
CompMap::CompMap(const CompMap& other) {
  init(other.basis());
  map_ = other.map();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
CompMap::~CompMap() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
CompMap::iterator CompMap::begin() {
  return map_.begin();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
CompMap::iterator CompMap::end() {
  return map_.end();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
double& CompMap::operator[](const int& tope) {
  normalized_ = false;
  return map_.operator[](tope);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
int CompMap::count(Iso tope) const {
  return map_.count(tope);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void CompMap::erase(Iso tope) {
  normalized_ = false;
  map_.erase(tope);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void CompMap::erase(CompMap::iterator position) {
  normalized_ = false;
  map_.erase(position);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
bool CompMap::empty() const {
  return map_.empty();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
int CompMap::size() const {
  return map_.size();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
bool CompMap::operator==(const CompMap& rhs) const {
  if ( size() != rhs.size() ) {
    return false;
  }
  for (const_iterator it = map_.begin(); it != map_.end(); it++) {
    if (rhs.count(it->first) == 0) {
      return false;
    }
    double val = rhs.massFraction(it->first) - massFraction(it->first); 
    if (abs(val) > EPS_FRACTION) {
      return false;
    }
  }
  return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
bool CompMap::operator<(const CompMap& rhs) const {
  return (ID_ < rhs.ID());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
bool CompMap::recorded() const {
  return (ID_ > 0);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
Basis CompMap::basis() const {
  return basis_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
Map CompMap::map() const {
  return map_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
int CompMap::ID() const {
  return ID_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
double CompMap::massFraction(const Iso& tope) const {
  if (count(tope) == 0) {
    return 0.0;
    //throw CycIndexException("This composition has no Iso: " + tope);
  }
  double factor = 1.0;
  if (basis_ != MASS) {
    factor = MT->gramsPerMol(tope) / mass_to_atom_ratio_;
  }
  return factor * map_.find(tope)->second;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
double CompMap::atomFraction(const Iso& tope) const {
  if (count(tope) == 0) {
    return 0.0;
    //throw CycIndexException("This composition has no Iso: " + tope);
  }
  double factor = 1.0;
  if (basis_ != ATOM) {
    factor = 1 / (MT->gramsPerMol(tope) / mass_to_atom_ratio_);
  }
  return factor * map_.find(tope)->second;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
CompMapPtr CompMap::parent() const {
  return parent_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
double CompMap::decay_time() const {
  return decay_time_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
bool CompMap::normalized() const {
  return normalized_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
double CompMap::mass_to_atom_ratio() const {
  return mass_to_atom_ratio_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
CompMapPtr CompMap::me() {
  return shared_from_this();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CompMapPtr CompMap::root_comp() {
  CompMapPtr child = me();
  while (child->parent()) {
    child = child->parent();
  }
  return child;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double CompMap::root_decay_time() {
  CompMapPtr child = me();
  double time = decay_time();
  while (child->parent()) {
    child = child->parent();
    time += child->decay_time();
  }
  return time;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void CompMap::massify() {
  change_basis(MASS);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void CompMap::atomify() {
  change_basis(ATOM);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void CompMap::normalize() {
  double sum = 0.0;
  double other_sum = 0.0;
  bool atom = (basis_ == ATOM);
  for (iterator it = map_.begin(); it != map_.end(); it++) {
    validateEntry(it->first,it->second);
    sum += it->second;
    if (atom) {
      other_sum += it->second * MT->gramsPerMol(it->first);
    }
    else {
      other_sum += it->second / MT->gramsPerMol(it->first);
    }
  }
  if (atom) {
    mass_to_atom_ratio_ = other_sum / sum;
  }
  else {
    mass_to_atom_ratio_ = sum / other_sum;
  }
  normalize(sum);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void CompMap::init(Basis b) {
  basis_ = b;
  map_ = Map();
  normalized_ = false;
  mass_to_atom_ratio_ = 1;
  ID_ = 0;
  decay_time_ = 0;
  parent_.reset();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void CompMap::change_basis(Basis b) {
  if (!normalized()) {
    normalize();
  }
  if (basis_ != b) { // only change if we have to
    for (iterator it = map_.begin(); it != map_.end(); it++) {
      switch (b) {
      case ATOM:
        map_[it->first] = atomFraction(it->first);
        break;
      case MASS:
        map_[it->first] = massFraction(it->first);
        break;
      default:
        throw CycRangeException("Basis not atom or mass.");
        break;
      }
    }
    basis_ = b;
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void CompMap::normalize(double sum) {
  if (sum != 1) { // only normalize if needed
    for (iterator it = map_.begin(); it != map_.end(); it++) {
      it->second /= sum;
    }
  }
  normalized_ = true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
int CompMap::getAtomicNum(Iso tope) {
  validateIsotopeNumber(tope);
  return tope / 1000; // integer division;
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
int CompMap::getMassNum(Iso tope) {
  validateIsotopeNumber(tope);
  return tope % 1000;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void CompMap::validate() {
  for (Map::iterator it = map_.begin(); it != map_.end(); it ++) {
    validateEntry(it->first,it->second);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void CompMap::validateEntry(const Iso& tope, const double& value) {
  validateIsotopeNumber(tope);
  validateValue(value);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void CompMap::validateIsotopeNumber(const Iso& tope) {
  int lower_limit = 1001;
  int upper_limit = 1182949;  
  if (tope < lower_limit || tope > upper_limit) {
    stringstream ss("");
    ss << tope;
    throw CycRangeException("Isotope identifier '" + ss.str() + "' is not valid.");
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void CompMap::validateValue(const double& value) {
  if (value < 0.0) {
    string err_msg = "CompMap has negative quantity for an isotope.";
    throw CycRangeException(err_msg);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void CompMap::print() {
  CLOG(log_level_) << detail();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::string CompMap::detail() {
  stringstream ss;
  vector<string> entries = compStrings();
  for (vector<string>::iterator entry = entries.begin(); 
       entry != entries.end(); entry++) {
    CLOG(log_level_) << *entry;
  }
  return "";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::vector<std::string> CompMap::compStrings() {
  stringstream ss;
  vector<string> comp_strings;
  for (const_iterator entry = map_.begin(); entry != map_.end(); entry++) {
    ss.str("");
    ss << entry->first << ": " << entry->second << " % / kg";
    comp_strings.push_back(ss.str());
  }
  return comp_strings;
}
