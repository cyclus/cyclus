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
RecipeMap* IsoVector::recipes_ = new RecipeMap();
CompMap* IsoVector::init_comp_ = new CompMap();
DecayChainMap* IsoVector::decay_chains_ = new DecayChainMap();
DecayTimesMap* IsoVector::decay_times_ = new DecayTimesMap();
table_ptr IsoVector::iso_table = new Table("IsotopicStates"); 

// memory for new isovectors to point to


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
IsoVector::IsoVector() {init();}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
IsoVector::IsoVector(CompMap* initial_comp) {
  init();
  validateComposition();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
IsoVector::IsoVector(CompMap* initial_comp, bool atom) {
  init();
  validateComposition();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void IsoVector::init() {
  composition_ = new composition(-1,init_comp_,1,1);
  decay_parent_composition_ = new composition();
  decay_parent_composition_->mass_fractions_ = init_comp_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
bool recipeLogged(std::string name) {
  return ( !(recipes_->count(name) == 0) ); // true iff name in recipes_
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void IsoVector::logRecipe(composition* recipe) {
    recipe->ID = nextStateID_++;
    addToTable(recipe);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void IsoVector::logRecipe(std::string name, composition* recipe) {
  if ( !recipeLogged(name) ) {
    // log this with the database
    logRecipe(recipe);

    // initialize containers
    set<int>* times = new set<int>();
    map<int, composition*> daughters = new map<int,composition*>();

    // assign containers
    (*recipes_)[name] = recipe;
    (*decay_times_)[recipe] = times;
    (*decay_chains_)[recipe] = daughters;
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
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
      atom_count += value * MT->gramsPerMol(int tope);
    }
    else if (basis == "atom") {
      atom_count += value;
      value = value / MT->gramsPerMol(int tope);
    }
    else {
      throw CycIOException(comp_type + " comp type is not 'mass' or 'atom'.");
    }

    // update our mass-related values
    mass_count += value;
    (*mass_fractions)[isotope] = value;
  }
  
  // make a new composition
  composition* comp = new composition();
  comp->mass_fractions = mass_fractions;
  comp->mass_normalizer = mass_count;
  comp->atom_normalizer = atom_count;

  // log this composition (static members and database)
  logRecipe(name,composition);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
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
      throw CycRangeException(format + "is not a supported recipebook format.");
    }
    XMLinput->stripCurNS();
  }
  CLOG(LEV_DEBUG2) << "}";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
IsoVector* IsoVector::recipe(std::string name) { 
  if ( !recipeLogged(name) ) {
    throw CycIndexException("Recipe '" + name + "' does not exist.");
  }
  return new IsoVector((*recipes_)[name]->mass_fractions);
} 
  
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void IsoVector::printRecipes() {
  CLOG(LEV_INFO1) << "There are " << IsoVector::recipeCount() << " recipes.";
  CLOG(LEV_INFO2) << "Recipe list {";
  for (map<string, IsoVector*>::iterator recipe=recipes_->begin();
      recipe != recipes_->end();
      recipe++){
    CLOG(LEV_INFO2) << "Recipe name=" << recipe->first;
    recipe->second->print();
  }
  CLOG(LEV_INFO2) << "}";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void IsoVector::print() {
  CLOG(LEV_INFO3) << detail();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::string IsoVector::detail() {
  stringstream ss;
  vector<string>::iterator entry;
  vector<string>* entries = compStrings();
  for (entry = entries->begin(); entry != entries->end(); entry++) {
    CLOG(LEV_INFO3) << *entry;
  }
  delete entries;
  return "";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::vector<std::string>* IsoVector::compStrings() {
  CompMap::iterator entry;
  int isotope;
  stringstream ss;
  vector<string>* comp_strings = new vector<string>();
  CompMap* comp = mass_comp();
  for (entry = comp->begin(); entry != comp->end(); entry++) {
    ss.str("");
    isotope = entry->first;
    if (mass(isotope) < EPS_KG) {continue;}
    ss << isotope << ": " << mass(isotope) << " kg";
    comp_strings->push_back(ss.str());
  }
  return comp_strings;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int IsoVector::recipeCount() { 
  return IsoVector::recipes_->size(); 
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
IsoVector IsoVector::operator+ (IsoVector rhs_vector) {
  int isotope;
  double rhs_atoms;
  CompMap sum_comp(atom_comp_);
  CompMap rhs_comp = rhs_vector.atom_comp_;

  CompMap::iterator rhs;
  for (rhs = rhs_comp.begin(); rhs != rhs_comp.end(); rhs++) {
    isotope = rhs->first;
    rhs_atoms = rhs->second;

    if (sum_comp.count(isotope) == 0) {
      sum_comp[isotope] = 0;
    }
    sum_comp[isotope] += rhs_atoms;
  }
  mass_out_of_date_ = true;

  IsoVector temp(sum_comp);

  // preserve composition parentage to prevent duplicate db recording
  if (rhs_vector.loggedComps_ == loggedComps_) {
    temp.loggedComps_ = loggedComps_;
    temp.decayTime_ = decayTime_;
  }

  return (temp);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
IsoVector IsoVector::operator- (IsoVector rhs_vector) {
  int isotope;
  double rhs_atoms;
  CompMap diff_comp(atom_comp_);
  CompMap rhs_comp = rhs_vector.atom_comp_;

  CompMap::iterator rhs;
  for (rhs = rhs_comp.begin(); rhs != rhs_comp.end(); rhs++) {
    isotope = rhs->first;
    rhs_atoms = rhs->second;

    if (diff_comp.count(isotope) == 0) {
      diff_comp[isotope] = 0;
    }

    if (rhs_atoms > diff_comp[isotope]) {
      throw CycRangeException("Attempted to extract more than exists.");
    }
    diff_comp[isotope] -= rhs_atoms;
  }

  IsoVector temp(diff_comp);

  // preserve composition parentage to prevent duplicate db recording
  if (rhs_vector.loggedComps_ == loggedComps_) {
    temp.loggedComps_ = loggedComps_;
    temp.decayTime_ = decayTime_;
  }

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
    if (diff > EPS_FRACTION) {
      return false;
    }
  }

  return true;
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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void IsoVector::executeDecay(double time_change) {
  double months_per_year = 12;
  double years = time_change / months_per_year;

  DecayHandler handler;
  handler.setComp(composition_->mass_fractions);
  handler.decay(years);
  atom_comp_ = handler.compAsCompMap();
  decayTime_ += time_change;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CompMap* IsoVector::mass_comp() {
  return composition_->mass_fractions_;
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
  double value = (*composition_->mass_fractions)[tope];
  return value / composition_->mass_normalizer;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double IsoVector::atomFraction(Iso tope) {
  double value = (*composition_->mass_fractions)[tope];
  return value * MT->gramsPerMol(int tope) / composition_->atom_normalizer;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool IsoVector::isZero(Iso tope) {
  validateIsotopeNumber(tope);
  if (composition_->mass_fractions->count(tope) == 0) {
    return true;
  }
  else {
    double fraction = atomFraction(tope);
    return (fraction < EPS_PERCENT);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void IsoVector::recordState() {
  if (parent_ == 0) {
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
  CompMap* comp = recipe->mass_fractions_;
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
    // record this primary key
    pkref_.push_back(id);
    pkref_.push_back(iso_id);
  }
}
