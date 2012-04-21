// IsoVector.cpp
#include "IsoVector.h"

#include "CycException.h"
#include "MassTable.h"
#include "Logger.h"
#include "InputXML.h"
#include "DecayHandler.h"

#include <cmath>
#include <sstream>
#include <iostream>
#include <vector>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>

using namespace std;

// Static variables to be initialized.
int IsoVector::nextStateID_ = 0;
RecipeMap IsoVector::recipes_;
DecayChainMap IsoVector::decay_chains_;
DecayTimesMap IsoVector::decay_times_;
CompMap* IsoVector::init_comp_ = new CompMap();
table_ptr IsoVector::iso_table = new Table("IsotopicStates"); 

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
  composition_ = new composition(init_comp_,1,1);
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
IsoVector IsoVector::operator+ (IsoVector rhs_vector) {
  int isotope;
  double rhs_fracs;
  double rhs_normalizer = rhs_vector.mass_normalizer();
  CompMap* rhs_comp = rhs_vector.mass_comp();
  CompMap* sum_comp = new CompMap(*mass_comp());

  CompMap::iterator rhs;
  for (rhs = rhs_comp->begin(); rhs != rhs_comp->end(); rhs++) {
    isotope = rhs->first;
    rhs_fracs = rhs->second;

    double value = rhs_fracs * rhs_normalizer / mass_normalizer();
    if (sum_comp->count(isotope) == 0) {
      (*sum_comp)[isotope] = value;
    }
    else {
      (*sum_comp)[isotope] += value;
    }
  }

  IsoVector temp(sum_comp);
  return (temp);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
IsoVector IsoVector::operator- (IsoVector rhs_vector) {
  int isotope;
  double rhs_fracs;
  double rhs_normalizer = rhs_vector.mass_normalizer();
  CompMap* rhs_comp = rhs_vector.mass_comp();
  CompMap* sum_comp = new CompMap(*mass_comp());

  CompMap::iterator rhs;
  for (rhs = rhs_comp->begin(); rhs != rhs_comp->end(); rhs++) {
    isotope = rhs->first;
    rhs_fracs = rhs->second;

    double value = rhs_fracs * rhs_normalizer / mass_normalizer();
    if (sum_comp->count(isotope) > 0) {
      if ( (*sum_comp)[isotope] > value) {
        (*sum_comp)[isotope] -= value;
      }
      else {
        (*sum_comp)[isotope] = 0;
      }
    }
  }
  IsoVector temp(sum_comp);
  return (temp);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool IsoVector::operator== (IsoVector rhs_vector) {
  int isotope;
  double diff;
  CompMap* this_comp = mass_comp();
  CompMap* rhs_comp = rhs_vector.mass_comp();
  
  if (this_comp->size() != rhs_comp->size() ) {
    return false;
  }

  CompMap::iterator rhs_iter;
  for (rhs_iter = rhs_comp->begin(); rhs_iter != rhs_comp->end(); rhs_iter++) {
    isotope = rhs_iter->first;
    if (this_comp->count(isotope) == 0) {
      return false;
    }
    diff = fabs(massFraction(isotope) - rhs_vector.massFraction(isotope));
    if (diff > EPS_PERCENT) {
      return false;
    }
  }

  return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
IsoVector operator* (IsoVector &v, double factor) {
  v.minimizeComposition();
  v.multMassNormBy(factor);
  return v;
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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
bool IsoVector::recipeLogged(std::string name) {
  // true iff name in recipes_
  int count = IsoVector::recipes_.count(name);
  return (count != 0); 
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void IsoVector::logRecipe(composition* recipe) {
    recipe->ID = nextStateID_++;
    addToTable(recipe);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void IsoVector::logRecipe(std::string name, composition* recipe) {
  if ( !recipeLogged(name) ) {
    // log this with the database
    logRecipe(recipe);
    // store this as a named recipe
    recipes_[name] = recipe;
    // store this as a decayable recipe
    storeDecayableRecipe(recipe);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void IsoVector::storeDecayableRecipe(composition* recipe) {
  // initialize containers
  decay_times* times = new decay_times();
  DaughterMap* daughters = new DaughterMap();
  // assign containers
  decay_times_[recipe] = times;
  decay_chains_[recipe] = daughters;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void IsoVector::logRecipeDecay(composition* parent, composition* child, 
                               int t_i, int t_f) {
  decay_times* times = decayTimes(parent);
  times->insert(t_f);
  DaughterMap* daughters = Daughters(parent);
  addDaughter(daughters,child,t_f);
  child->parent = parent;
  child->decay_time = t_f - t_i;
  logRecipe(child);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void IsoVector::load_recipe(xmlNodePtr cur) {
  // initialize comp map
  CompMap* mass_fractions = new CompMap();

  // get general values from xml
  string name = XMLinput->get_xpath_content(cur,"name");
  string basis = XMLinput->get_xpath_content(cur,"basis");
  xmlNodeSetPtr isotopes = XMLinput->get_xpath_elements(cur,"isotope");

  // get values needed for composition
  double value;
  double mass_count = 0, atom_count = 0;
  int key;
  xmlNodePtr iso_node;
  for (int i = 0; i < isotopes->nodeNr; i++) {
    iso_node = isotopes->nodeTab[i];
    key = strtol(XMLinput->get_xpath_content(iso_node,"id"), NULL, 10);
    value = strtod(XMLinput->get_xpath_content(iso_node,"comp"), NULL);

    if (basis == "mass") {
      atom_count += value * MT->gramsPerMol(key);
    }
    else if (basis == "atom") {
      atom_count += value;
      value = value / MT->gramsPerMol(key);
    }
    else {
      throw CycIOException(basis + " basis is not 'mass' or 'atom'.");
    }

    // update our mass-related values
    mass_count += value;
    (*mass_fractions)[key] = value;
  }
  
  // make a new composition
  composition* comp = new composition(mass_fractions,mass_count,atom_count);

  // log this composition (static members and database)
  logRecipe(name,comp);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void IsoVector::load_recipes() {
  // load recipes from file
  xmlNodeSetPtr nodes = XMLinput->get_xpath_elements("/*/recipe");
  string name;
  CLOG(LEV_DEBUG2) << "loading recipes {";
  for (int i = 0; i < nodes->nodeNr; i++) {
    name = XMLinput->getCurNS() + 
                  XMLinput->get_xpath_content(nodes->nodeTab[i], "name");
    CLOG(LEV_DEBUG2) << "Adding recipe '" << name << "'.";
    load_recipe(nodes->nodeTab[i]); // load recipe
  }

  // load recipes from databases
  nodes = XMLinput->get_xpath_elements("/*/recipebook");
  string filename, ns, format;
  for (int i = 0; i < nodes->nodeNr; i++) {
    filename = XMLinput->get_xpath_content(nodes->nodeTab[i], "filename");
    ns = XMLinput->get_xpath_content(nodes->nodeTab[i], "namespace");
    format = XMLinput->get_xpath_content(nodes->nodeTab[i], "format");
    XMLinput->extendCurNS(ns);

    if ("xml" == format) {
      CLOG(LEV_DEBUG2) << "going into a recipe book...";
      XMLinput->load_recipebook(filename);  // load recipe book
    } 
    else {
      throw 
        CycRangeException(format + "is not a supported recipebook format.");
    }
    XMLinput->stripCurNS();
  }
  CLOG(LEV_DEBUG2) << "}";
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
int IsoVector::recipeCount() { 
  return IsoVector::recipes_.size(); 
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
composition* IsoVector::recipe(std::string name) { 
  if ( !recipeLogged(name) ) {
    throw CycIndexException("Recipe '" + name + "' does not exist.");
  }
  return recipes_[name];
} 

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
decay_times* IsoVector::decayTimes(composition* parent) {
  return decay_times_[parent];
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
DaughterMap* IsoVector::Daughters(composition* parent) {
  return decay_chains_[parent];
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
composition* IsoVector::Daughter(composition* parent, int time) {
  return (*Daughters(parent))[time];
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void IsoVector::addDaughter(DaughterMap* dmap, composition* d, int time) {
  (*dmap)[time] = d;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int IsoVector::getAtomicNum(Iso tope) {
  validateIsotopeNumber(tope);
  return tope / 1000; // integer division
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int IsoVector::getMassNum(Iso tope) {
  validateIsotopeNumber(tope);
  return tope % 1000;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void IsoVector::delete_comp() {
  if (!logged()) {
    composition_->delete_map();
  }
  delete composition_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void IsoVector::executeDecay(double time_change) {
  composition* p = parent();
  if (p->logged()) { // check for duplicate decay isotopics
    decay_times* times = decayTimes(p);
    int t_i = decay_time();
    int t_f = t_i + time_change;
    if (times->find(t_f) != times->end()) { // decay isotopics already exist
      composition* daughter = Daughter(p,t_f);
      this->setComposition(daughter);
    }
    else { // create and log new isotopics
      executeDecay(time_change,composition_); // changes composition_
      logRecipeDecay(p,composition_,t_i,t_f);
    }
  } // end p->logged
  else {
    executeDecay(time_change,composition_);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void IsoVector::executeDecay(double time_change, composition* child) {
  // get decay time
  double months_per_year = 12;
  double years = time_change / months_per_year;

  // perform decay
  DecayHandler handler;
  composition* parent = child;
  CompMap* mass_comp = child->mass_fractions; 
  atomify(mass_comp);
  handler.setComp(*mass_comp);
  handler.decay(years);
  CompMap* comp = new CompMap(handler.compAsCompMap());
  this->setComposition(comp,true); // changes composition_
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
composition* IsoVector::parent() {
  composition* child = composition_;
  while (child->parent != 0) {
    child = child->parent;
  }
  return child;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void IsoVector::validateComposition() {
  int tope;
  double fraction;
  CompMap* fractions = composition_->mass_fractions;
  for (CompMap::iterator comp_iter = fractions->begin(); 
       comp_iter != fractions->end(); comp_iter++) {
    // isotope number
    tope = comp_iter->first;
    validateIsotopeNumber(tope);
    // mass fraction
    fraction = comp_iter->second;
    validateFraction(fraction);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void IsoVector::validateFraction(double fraction) {
    if (fraction < 0.0) {
      string err_msg = "Composition has negative quantity for an isotope.";
      throw CycRangeException(err_msg);
    }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void IsoVector::validateIsotopeNumber(Iso tope) {
  int lower_limit = 1001;
  int upper_limit = 1182949;

  if (tope < lower_limit || tope > upper_limit) {
    stringstream st;
    st << tope;
    string isotope = st.str();
    throw CycRangeException("Isotope identifier '" + isotope + "' is not valid.");
  }
}

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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void IsoVector::define_table() {
  // declare the state id columns and add it to the table
  column state_id("ID","INTEGER");
  column iso_id("IsoID","INTEGER");
  column iso_value("Value","REAL");
  iso_table->addColumn(state_id);
  iso_table->addColumn(iso_id);
  iso_table->addColumn(iso_value);
  // declare the table's primary key
  primary_key pk;
  pk.push_back("ID"), pk.push_back("IsoID");
  iso_table->setPrimaryKey(pk);
  // we've now defined the table
  iso_table->tableDefined();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void IsoVector::addToTable(composition* recipe){
  // if we haven't logged a composition yet, define the table
  if ( !iso_table->defined() ) {
    IsoVector::define_table();
  }

  // make a row - stateid first then isotopics
  // declare data
  data an_id(recipe->ID);
  // declare entries
  entry id("ID",an_id);

  // now for the composition isotopics
  CompMap* comp = recipe->mass_fractions;
  int i = 0;
  for (CompMap::iterator item = comp->begin();
       item != comp->end(); item++){
    CLOG(LEV_DEBUG2) << "isotope " << i++ << " of " << comp->size();
    // declare row
    // decalre data
    data an_iso_id(item->first), an_iso_value(item->second);
    // declare entries
    entry iso_id("IsoID",an_iso_id), iso_value("Value",an_iso_value);
    // construct row
    row aRow;
    aRow.push_back(id), aRow.push_back(iso_id), aRow.push_back(iso_value);
    // add the row
    iso_table->addRow(aRow);
    // // record this primary key
    // pkref_.push_back(id);
    // pkref_.push_back(iso_id);
  }
}

// //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// primary_key_ref IsoVector::pkref() {
//   return pkref_;
// }
