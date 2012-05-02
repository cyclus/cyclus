// IsoVector.cpp
#include "IsoVector.h"

#include "CycException.h"
#include "RecipeLogger.h"
#include "Logger.h"
#include "DecayHandler.h"

#include <sstream>

using namespace std;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
IsoVector::IsoVector() {
  init();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
IsoVector::IsoVector(composition* comp) {
  init();
  setComposition(comp);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
IsoVector::IsoVector(CompMap* initial_comp) {
  init();
  setComposition(initial_comp);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
IsoVector::IsoVector(CompMap* initial_comp, bool atom) {
  init();
  setComposition(initial_comp,atom);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
IsoVector::~IsoVector() {
  this->delete_comp();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void IsoVector::init() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void IsoVector::massify(CompMap* comp) {
  for (CompMap::iterator ci = comp->begin(); ci != comp->end(); ci++) {
    ci->second *= MT->gramsPerMol(ci->first);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void IsoVector::atomify(CompMap* comp) {
  for (CompMap::iterator ci = comp->begin(); ci != comp->end(); ci++) {
    ci->second /= MT->gramsPerMol(ci->first);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void IsoVector::setComposition(composition* c) {
  minimizeComposition(c); // c guaranteed to be mass basis
  this->delete_comp();
  composition_ = 
    new composition(c->mass_fractions,c->mass_normalizer,c->atom_normalizer);
  validateComposition();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void IsoVector::setComposition(CompMap* comp) {
  // comp guaranteed to be mass basis
  pair<double,double> normalizers = getNormalizers(comp);  
  composition* c = new composition(comp,normalizers.first,normalizers.second);
  setComposition(c);
  delete c;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void IsoVector::setComposition(CompMap* comp, bool atom) {
  if (atom) {
    massify(comp);
  }
  setComposition(comp);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
std::pair<double,double> IsoVector::getNormalizers(CompMap* comp) {
  double mass_norm = 0, atom_norm = 0;
  for (CompMap::iterator ci = comp->begin(); ci != comp->end(); ci++) {
    double value = ci->second;
    mass_norm += value;
    atom_norm += value * MT->gramsPerMol(ci->first);
  }
  pair<double,double> normalizers(mass_norm,atom_norm);
  return normalizers;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void IsoVector::minimizeComposition() {
  minimizeComposition(composition_);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void IsoVector::minimizeComposition(composition* c) {
  CompMap* fractions = c->mass_fractions;
  for (CompMap::iterator ci = fractions->begin(); ci != fractions->end(); 
       ci++) {
    ci->second /= c->mass_normalizer;
  }
  c->atom_normalizer /= c->mass_normalizer;
  c->mass_normalizer = 1.0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void IsoVector::multMassNormBy(double factor) {
  multMassNormBy(composition_,factor);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void IsoVector::multMassNormBy(composition* c, double factor) {
  c->mass_normalizer *= factor;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
IsoVector operator* (IsoVector &v, double factor) {
  IsoVector result = *this;
  result.multMassNormBy(factor);
  return resutl;
}
IsoVector operator* (double factor, IsoVector &v) {
  return operator*(v,factor);
}
IsoVector operator* (IsoVector &v, int factor) {
  return operator*(v, (double) factor);
}
IsoVector operator* (int factor, IsoVector &v) {
  return operator*(v, (double) factor);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
IsoVector operator/ (IsoVector &v, double factor) {
  return operator*(v,1/factor);
}
IsoVector operator/ (IsoVector &v, int factor) {
  double f = (double) factor;
  return operator*(v,1/f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool IsoVector::logged() { 
  bool condition1 = mass_comp() != init_comp_; 
  bool condition2 = composition_->logged();
  bool logged = condition1 && condition2;
  return logged;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void IsoVector::printRecipes() {
  CLOG(LEV_INFO1) << "There are " << IsoVector::recipeCount() << " recipes.";
  CLOG(LEV_INFO2) << "Recipe list {";
  for (RecipeMap::iterator recipe=recipes_.begin();
      recipe != recipes_.end();
      recipe++){
    CLOG(LEV_INFO2) << "Recipe name=" << recipe->first;
    print(recipe->second);
  }
  CLOG(LEV_INFO2) << "}";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void IsoVector::print(composition* c) {
  CLOG(LEV_INFO3) << detail(c);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void IsoVector::print() {
  print(composition_);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::string IsoVector::detail(composition* c) {
  stringstream ss;
  vector<string>::iterator entry;
  vector<string>* entries = compStrings(c);
  for (entry = entries->begin(); entry != entries->end(); entry++) {
    CLOG(LEV_INFO3) << *entry;
  }
  delete entries;
  return "";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::vector<std::string>* IsoVector::compStrings(composition* c) {
  CompMap::iterator entry;
  int isotope;
  stringstream ss;
  vector<string>* comp_strings = new vector<string>();
  CompMap* comp = c->mass_fractions;
  for (entry = comp->begin(); entry != comp->end(); entry++) {
    ss.str("");
    isotope = entry->first;
    if (massFraction(isotope,c) >= EPS_PERCENT) {
      ss << isotope << ": " << entry->second / c->mass_normalizer << " % / kg";
      comp_strings->push_back(ss.str());
    }
  }
  return comp_strings;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void IsoVector::executeDecay(double time_change, composition* child) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int IsoVector::stateID() {
  return composition_->ID;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CompMap* IsoVector::mass_comp() {
  return composition_->mass_fractions;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int IsoVector::decay_time() {
  composition* child = composition_;
  double decay_time = 0.0;
  while (child->parent != 0) {
    decay_time += child->decay_time;
    child = child->parent;
  }
  return decay_time;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
comp_t* IsoVector::root_comp() {
  comp_t* child = composition_;
  while (child->parent != 0) {
    child = child->parent;
  }
  return child;
}
;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double IsoVector::mass_normalizer() {
  return composition_->mass_normalizer;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double IsoVector::massFraction(Iso tope) {
  return massFraction(tope,composition_);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double IsoVector::massFraction(Iso tope, composition* c) {
  double value = (*c->mass_fractions)[tope];
  return value / c->mass_normalizer;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double IsoVector::atomFraction(Iso tope) {
  return atomFraction(tope,composition_);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double IsoVector::atomFraction(Iso tope, composition* c) {
  double value = (*c->mass_fractions)[tope];
  return value * MT->gramsPerMol(tope) / c->atom_normalizer;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool IsoVector::isZero(Iso tope) {
  validateIsotopeNumber(tope);
  if (composition_->mass_fractions->count(tope) == 0) {
    return true;
  }
  else {
    double fraction = atomFraction(tope);
    validateFraction(fraction);
    return (fraction < EPS_PERCENT);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void IsoVector::recordState() {
  if (composition_->ID == 0) {
    logRecipe(composition_);
  }
}
