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
int IsoVector::nextID_ = 0;
int IsoVector::nextStateID_ = 0;
std::map<std::string, IsoVector*> IsoVector::recipes_;
StateMap IsoVector::predefinedStates_ = StateMap();
// Database table for isotopic states
Table *IsoVector::iso_table = new Table("IsotopicStates"); 


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
IsoVector::IsoVector() {
  ID_ = nextID_++;
  mass_out_of_date_ = true;
  total_mass_ = 0;
  trackComposition();
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
IsoVector::IsoVector(CompMap initial_comp) {
  ID_ = nextID_++;
  atom_comp_ = initial_comp;
  total_mass_ = 0;
  mass_out_of_date_ = true;

  validateComposition();
  trackComposition();
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
IsoVector::IsoVector(xmlNodePtr cur) {
  ID_ = nextID_++;
  total_mass_ = 0;
  mass_out_of_date_ = true;

  string recipe_name = XMLinput->get_xpath_content(cur,"name");
  string comp_type = XMLinput->get_xpath_content(cur,"basis");
  xmlNodeSetPtr isotopes = XMLinput->get_xpath_elements(cur,"isotope");

  double atom_count, grams_per_atom;
  int isotope;
  xmlNodePtr iso_node;
  for (int i = 0; i < isotopes->nodeNr; i++) {
    iso_node = isotopes->nodeTab[i];
    isotope = strtol(XMLinput->get_xpath_content(iso_node,"id"), NULL, 10);

    grams_per_atom = MT->getMassInGrams(isotope);
    atom_count = strtod(XMLinput->get_xpath_content(iso_node,"comp"), NULL);

    if ( "atom" != comp_type) {
      atom_count *= grams_per_atom;
    }

    atom_comp_[isotope] = strtod(XMLinput->get_xpath_content(iso_node,"comp"), NULL);
  }

  double total_qty = strtol(XMLinput->get_xpath_content(cur,"total"), NULL, 10);
  if ("atom" == comp_type) {
    setAtomCount(total_qty);
  } else {
    setMass(total_qty);
  }
  trackComposition();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void IsoVector::load_recipes() {

  /// load recipes from file
  xmlNodeSetPtr nodes = XMLinput->get_xpath_elements("/*/recipe");
  std::string name;
  CLOG(LEV_DEBUG2) << "loading recipes {";
  for (int i = 0; i < nodes->nodeNr; i++) {
    name = XMLinput->getCurNS() + 
                  XMLinput->get_xpath_content(nodes->nodeTab[i], "name");
    CLOG(LEV_DEBUG2) << "Adding recipe '" << name << "'.";
    recipes_[name] = new IsoVector(nodes->nodeTab[i]);
  }

  /// load recipes from databases
  nodes = XMLinput->get_xpath_elements("/*/recipebook");
  std::string filename, ns, format;

  for (int i = 0; i < nodes->nodeNr; i++) {
    filename = XMLinput->get_xpath_content(nodes->nodeTab[i], "filename");
    ns = XMLinput->get_xpath_content(nodes->nodeTab[i], "namespace");
    format = XMLinput->get_xpath_content(nodes->nodeTab[i], "format");
    XMLinput->extendCurNS(ns);

    if ("xml" == format) {
      CLOG(LEV_DEBUG2) << "going into a recipe book...";
      XMLinput->load_recipebook(filename);
    } else {
      throw CycRangeException(format + "is not a supported recipebook format.");
    }
    XMLinput->stripCurNS();
  }
  CLOG(LEV_DEBUG2) << "}";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
IsoVector IsoVector::recipe(std::string name) { 
  if (recipes_.count(name) == 0) {
    throw CycIndexException("Recipe '" + name + "' does not exist.");
  }
  return *(recipes_[name]);
} 
  
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void IsoVector::printRecipes() {
  CLOG(LEV_INFO1) << "There are " << IsoVector::recipeCount() << " recipes.";
  CLOG(LEV_INFO2) << "Recipe list {";
  for (std::map<std::string, IsoVector*>::iterator recipe=recipes_.begin();
      recipe != recipes_.end();
      recipe++){
    CLOG(LEV_INFO2) << "Recipe name=" << recipe->first;
    recipe->second->print();
  }
  CLOG(LEV_INFO2) << "}";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void IsoVector::print() {
  CLOG(LEV_INFO3) << propString();

  std::vector<std::string>::iterator entry;
  std::vector<std::string> entries = compStrings();
  for (entry = entries.begin(); entry != entries.end(); entry++) {
    CLOG(LEV_INFO3) << *entry;
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::string IsoVector::propString() {
  std::stringstream ss;
  ss << "mass = " << mass() << " kg";
  return ss.str();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::vector<std::string> IsoVector::compStrings() {
  CompMap::iterator entry;
  int isotope;
  std::stringstream ss;
  std::vector<std::string> comp_strings;
  for (entry = atom_comp_.begin(); entry != atom_comp_.end(); entry++) {
    ss.str("");
    isotope = entry->first;
    if (mass(isotope) < EPS_KG) {continue;}
    ss << isotope << ": " << mass(isotope) << " kg";
    comp_strings.push_back(ss.str());
  }
  return comp_strings;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int IsoVector::recipeCount() { 
  return IsoVector::recipes_.size(); 
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
  return (temp);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool IsoVector::operator== (IsoVector rhs_vector) {
  int isotope;
  double mass_diff;
  CompMap this_comp(atom_comp_);
  CompMap rhs_comp = rhs_vector.atom_comp_;

  CompMap::iterator rhs_iter, this_iter;
  for (rhs_iter = rhs_comp.begin(); rhs_iter != rhs_comp.end(); rhs_iter++) {
    isotope = rhs_iter->first;
    mass_diff = fabs(rhs_vector.mass(isotope) - mass(isotope));
    if (mass_diff > EPS_KG) {
      return false;
    }
  }

  for (this_iter = this_comp.begin(); this_iter != this_comp.end(); this_iter++) {
    isotope = this_iter->first;
    mass_diff = fabs(rhs_vector.mass(isotope) - mass(isotope));
    if (mass_diff > EPS_KG) {
      return false;
    }
  }

  if (fabs(rhs_vector.mass() - mass()) > EPS_KG) {
    return false;
  }
  return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int IsoVector::getAtomicNum(int tope) {
  validateIsotopeNumber(tope);
  return tope / 1000; // integer division
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int IsoVector::getMassNum(int tope) {
  validateIsotopeNumber(tope);
  return tope % 1000;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double IsoVector::mass() {
  if (mass_out_of_date_) {
    total_mass_ = 0;
    int isotope;

    map<int, double>::const_iterator iter = atom_comp_.begin();
    while (iter != atom_comp_.end()) {
      isotope = iter->first;
      total_mass_ += mass(isotope);
      iter++;
    }
    mass_out_of_date_ = false;
  }

  return total_mass_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double IsoVector::mass(int tope) {
  if (isZero(tope)) {
    return 0;
  }

  int grams_per_kg = 1000;
  double grams_per_mole = MT->getMassInGrams(tope);

  return atom_comp_[tope] * grams_per_mole / grams_per_kg;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void IsoVector::setMass(double new_mass) {
  int isotope;
  double curr_mass = mass();

  if (curr_mass < EPS_KG) {
    string err_msg = "Cannot set mass for IsoVector with ";
    err_msg += "undefined (zero mass) composition.";
    throw CycRangeException(err_msg);
  } else if (new_mass < -1.0 * EPS_KG) {
    string err_msg = "Cannot set mass to a negative value.";
    throw CycRangeException(err_msg);
  }

  double ratio = new_mass / mass();

  map<int, double>::const_iterator iter = atom_comp_.begin();
  while (iter != atom_comp_.end()) {
    isotope = iter->first;
    atom_comp_[isotope] = atom_comp_[isotope] * ratio;
    iter++;
  }
  mass_out_of_date_ = false;
  total_mass_ = new_mass;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void IsoVector::setMass(int tope, double new_mass) {
  validateIsotopeNumber(tope);

  if (new_mass < -1.0 * EPS_KG) {
    string err_msg = "Cannot set isotope mass to a negative value.";
    throw CycRangeException(err_msg);
  }

  if (!mass_out_of_date_) {
    total_mass_ += new_mass - mass(tope);
  }

  int grams_per_kg = 1000;
  double grams_per_atom = MT->getMassInGrams(tope);
  atom_comp_[tope] = new_mass * grams_per_kg / grams_per_atom;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void IsoVector::multBy(double factor) {
  int isotope;
  map<int, double>::const_iterator iter = atom_comp_.begin();
  while (iter != atom_comp_.end()) {
    isotope = iter->first;
    atom_comp_[isotope] = atom_comp_[isotope] * factor;
    iter++;
  }

  if (!mass_out_of_date_) {
    total_mass_ *= factor;
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double IsoVector::atomCount() {
  double count = 0;
  int isotope;

  map<int, double>::const_iterator iter = atom_comp_.begin();
  while (iter != atom_comp_.end()) {
    isotope = iter->first;
    count += atomCount(isotope);
    iter++;
  }
  return count;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double IsoVector::atomCount(int tope) {
  validateIsotopeNumber(tope);
  if (isZero(tope)) {
    return 0;
  }

  return atom_comp_[tope];
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void IsoVector::setAtomCount(double new_count) {
  double curr_mass = mass();

  if (curr_mass < EPS_KG) {
    string err_msg = "Cannot set atom count for IsoVector with ";
    err_msg += "undefined (zero mass) composition.";
    throw CycRangeException(err_msg);
  } else if (new_count < 0.0) {
    string err_msg = "Cannot set atom count to a negative value.";
    throw CycRangeException(err_msg);
  }

  int isotope;
  double ratio = new_count / atomCount();

  map<int, double>::const_iterator iter = atom_comp_.begin();
  while (iter != atom_comp_.end()) {
    isotope = iter->first;
    atom_comp_[isotope] = atom_comp_[isotope] * ratio;
    iter++;
  }

  mass_out_of_date_ = true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void IsoVector::setAtomCount(int tope, double new_count) {
  validateIsotopeNumber(tope);
  double grams_per_kg = 1000;
  if (new_count * MT->getMassInGrams(tope) / grams_per_kg < -1 * EPS_KG) {
    string err_msg = "Cannot set isotope atom count to a negative value.";
    throw CycRangeException(err_msg);
  }

  atom_comp_[tope] = new_count;

  mass_out_of_date_ = true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double IsoVector::eltMass(int elt) {

  double elt_mass = 0;
  int isotope;
  int atomic_num;

  map<int, double>::const_iterator iter = atom_comp_.begin();
  while (iter != atom_comp_.end()) {

    isotope = iter->first;
    atomic_num = IsoVector::getAtomicNum(isotope);

    if (atomic_num == elt) {
      elt_mass += mass(isotope);
    }
    iter++;
  }

  return elt_mass;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void IsoVector::executeDecay(double time_change) {
  double months_per_year = 12;
  double years = time_change / months_per_year;


  DecayHandler handler;
  handler.setComp(atom_comp_);
  handler.decay(years);
  atom_comp_ = handler.compAsCompMap();
  this->trackComposition();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CompMap IsoVector::comp() {return atom_comp_;}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void IsoVector::validateComposition() {
  int grams_per_kg = 1000;
  int tope;
  double num_atoms, atoms_eps;

  CompMap::iterator comp_iter;
  for (comp_iter = atom_comp_.begin();
       comp_iter != atom_comp_.end();
       comp_iter++) {

    tope = comp_iter->first;
    num_atoms = comp_iter->second;

    validateIsotopeNumber(tope);

    atoms_eps =  EPS_KG * grams_per_kg / MT->getMassInGrams(tope); 
    if (num_atoms < -1 * atoms_eps) {
      string err_msg = "Composition has negative quantity for an isotope.";
      throw CycRangeException(err_msg);
    }
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void IsoVector::validateIsotopeNumber(int tope) {
  int lower_limit = 1001;
  int upper_limit = 1182949;

  if (tope < lower_limit || tope > upper_limit) {
    std::stringstream st;
    st << tope;
    std::string isotope = st.str();
    throw CycRangeException("Isotope identifier '" + isotope + "' is not valid.");
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool IsoVector::isZero(int tope) {
  validateIsotopeNumber(tope);

  if (atom_comp_.count(tope) == 0) {
    return true;
  }

  int grams_per_kg = 1000;
  double grams_per_mole = MT->getMassInGrams(tope) ; 

  return (atom_comp_[tope] * grams_per_mole / grams_per_kg < EPS_KG);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int IsoVector::compositionIsTracked() {
  CompMap* comp = &atom_comp_;
  StateMap::iterator lb = predefinedStates_.lower_bound(comp);

  if(lb != predefinedStates_.end() && !(predefinedStates_.key_comp()(comp, lb->first)))
    return lb->second; // found, return the state
  else if ( ( (int) comp->size() > 0 ) )
    return -1; // not found, not empty, return a corresponding token
  else
    return -2; // not found, but comp map is empty -- probably from empty constructor  
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void IsoVector::trackComposition() {
  int check = compositionIsTracked();
  if ( check == -1 ) {
    // this is a new composition, log it accordingly
    CompMap* comp = &atom_comp_;
    stateID_ = nextStateID_++;
    predefinedStates_[comp] = stateID_;
    IsoVector::addToTable();
  } else {
    // we are already tracking this composition
    stateID_ = check;
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
void IsoVector::addToTable(){
  // if we haven't logged an agent yet, define the table
  if ( !iso_table->defined() )
    IsoVector::define_table();

  // make a row - stateid first then isotopics
  // declare data
  data an_id( this->stateID() );
  // declare entries
  entry id("ID",an_id);

  // now for the composition isotopics
  CompMap* comp = &atom_comp_;
  for (CompMap::iterator item = comp->begin();
       item != comp->end(); item++){
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
