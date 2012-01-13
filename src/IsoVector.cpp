// IsoVector.cpp
#include "IsoVector.h"

#include "CycException.h"
#include "MassTable.h"
#include "Env.h"
#include "UniformTaylor.h"
#include "Logger.h"
#include "InputXML.h"
#include "MassTable.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>

using namespace std;

// Static variables to be initialized.
ParentMap IsoVector::parent_ = ParentMap();
DaughtersMap IsoVector::daughters_ = DaughtersMap();
Matrix IsoVector::decayMatrix_ = Matrix();
int IsoVector::nextID_ = 0;
std::map<std::string, IsoVector*> IsoVector::recipes_;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
IsoVector::IsoVector() {
  ID_ = nextID_++;
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
IsoVector::IsoVector(CompMap initial_comp) {
  ID_ = nextID_++;
  atom_comp_ = initial_comp;

  validateComposition();
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
IsoVector::IsoVector(xmlNodePtr cur) {
  ID_ = nextID_++;
  
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

  IsoVector::recipes_[recipe_name] = this;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void IsoVector::load_recipes() {

  /// load recipes from file
  xmlNodeSetPtr nodes = XMLinput->get_xpath_elements("/*/recipe");
  std::string name;
  for (int i = 0; i < nodes->nodeNr; i++) {
    name = XMLinput->getCurNS() + 
                  XMLinput->get_xpath_content(nodes->nodeTab[i], "name");
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
      XMLinput->load_recipebook(filename);
    } else {
      throw CycRangeException(format + "is not a supported recipebook format.");
    }
    XMLinput->stripCurNS();
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
IsoVector IsoVector::recipe(std::string name) { 
  if (recipes_.count(name) == 0) {
      throw CycIndexException("Recipe '" + name 
          + "' does not exist.");
  }
  return *(recipes_[name]);
} 
  
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void IsoVector::printRecipes() {
  for (std::map<std::string, IsoVector*>::iterator recipe=recipes_.begin();
      recipe != recipes_.end();
      recipe++){
    LOG(LEV_DEBUG2) << "Recipe " << recipe->first << ":";
    recipe->second->print();
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void IsoVector::print() {
  CompMap::iterator entry;
  int isotope;
  LOG(LEV_DEBUG2) << "    mass " << mass() << " kg";
  for (entry = atom_comp_.begin(); entry != atom_comp_.end(); entry++) {
    isotope = entry->first;
    LOG(LEV_DEBUG2) << "    " << isotope << ": "
                    << mass(isotope) << "kg";
  }
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
void IsoVector::loadDecayInfo() {
  string path = ENV->getCyclusPath() + "/Data/decayInfo.dat";
  ifstream decayInfo (path.c_str());

  if ( decayInfo.is_open() ) {
    int jcol = 1;
    int iso = 0;
    int nDaughters = 0;
    double decayConst = 0; // decay constant, in inverse years
    double branchRatio = 0;
      
    decayInfo >> iso;  // get first parent
    
    // checks to see if there are isotopes in 'decayInfo.dat'
    if ( decayInfo.eof() ) {
      string err_msg = "There are no isotopes in the 'decayInfo.dat' file";
      throw CycParseException(err_msg);
    }
    
    // processes 'decayInfo.dat'
    while ( !decayInfo.eof() ) {
      // make parent
      decayInfo >> decayConst;
      decayInfo >> nDaughters;
     
      // checks for duplicate parent isotopes
      if ( parent_.find(iso) == parent_.end() ) {
        parent_[iso] = make_pair(jcol, decayConst);
           
        // make daughters
        vector< pair<int,double> > temp(nDaughters);
   
        for ( int i = 0; i < nDaughters; ++i ) {
          decayInfo >> iso;
          decayInfo >> branchRatio;

          // checks for duplicate daughter isotopes
          for ( int j = 0; j < nDaughters; ++j ) {
            if ( temp[j].first == iso ) {
              throw CycParseException(string("A duplicate daughter isotope, %i , was found in decayInfo.dat", iso));
            } 
          }
          temp[i] = make_pair(iso, branchRatio);
        }
           
        daughters_[jcol] = temp;
        ++jcol; // set next column
      } else {
        string err_msg;
        err_msg = "A duplicate parent isotope was found in 'decayInfo.dat'";
        throw CycParseException(err_msg);
      }
      decayInfo >> iso; // get next parent
    } 
    // builds the decay matrix from the parent and daughter maps
    buildDecayMatrix();
  } else {
    throw CycIOException("Could not find file 'decayInfo.dat'.");
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double IsoVector::mass() {
  double mass_val = 0;
  int isotope;

  map<int, double>::const_iterator iter = atom_comp_.begin();
  while (iter != atom_comp_.end()) {
    isotope = iter->first;
    mass_val += mass(isotope);
    iter++;
  }

  return mass_val;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double IsoVector::mass(int tope) {
  validateIsotopeNumber(tope);
  if (isZero(tope)) {
    return 0;
  }

  double grams_per_atom;
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
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void IsoVector::setMass(int tope, double new_mass) {
  validateIsotopeNumber(tope);

  if (new_mass < -1.0 * EPS_KG) {
    string err_msg = "Cannot set isotope mass to a negative value.";
    throw CycRangeException(err_msg);
  }

  double grams_per_atom;
  int grams_per_kg = 1000;
  grams_per_atom = MT->getMassInGrams(tope);
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

  Vector N_o = compositionAsVector();
  buildDecayMatrix(); // update the decay matrix

  // solves the decay equation for the final composition
  Vector N_t = UniformTaylor::matrixExpSolver(decayMatrix_, N_o, years);

  copyVectorIntoComp(N_t);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CompMap IsoVector::comp() {return atom_comp_;}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Vector IsoVector::compositionAsVector() {
        
  Vector comp_vector = Vector(parent_.size(),1);

  map<int, double>::const_iterator comp_iter = atom_comp_.begin();
  while( comp_iter != atom_comp_.end() ) {
    int iso = comp_iter->first;
    long double atom_count = comp_iter->second;

    // if the isotope is tracked in the decay matrix  
    if ( parent_.count(iso) > 0 ) {
      int col = parent_.find(iso)->second.first; // get Vector position
      comp_vector(col,1) = atom_count;
    // if it is not in the decay matrix, then it is added as a stable isotope
    } else {
      double decayConst = 0;
      int col = parent_.size() + 1;
      parent_[iso] = make_pair(col, decayConst);  // add isotope to parent map

      int nDaughters = 0;
      vector< pair<int,double> > temp(nDaughters);
      daughters_[col] = temp;  // add isotope to daughters map
      
      vector<long double> row(1, atom_count);
      comp_vector.addRow(row);  // add isotope to the end of the Vector
    }

    ++comp_iter; // get next isotope
  }

  return comp_vector;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void IsoVector::copyVectorIntoComp(const Vector & compVector) {
  atom_comp_.clear();

  // loops through the ParentMap and populates the new composition map with
  // the number density from the compVector parameter for each isotope
  ParentMap::const_iterator parent_iter = parent_.begin(); // get first parent
  while( parent_iter != parent_.end() ) {
    int iso = parent_iter->first;
    int col = parent_.find(iso)->second.first; // get Vector position
    
    // checks to see if the Vector position is valid
    if ( col <= compVector.numRows() ) {
      double atom_count = compVector(col,1);
      // adds isotope to the map if its number density is non-zero
      if ( atom_count != 0 )
        atom_comp_[iso] = atom_count;
    } else {
      LOG(LEV_ERROR) << "Decay Error - invalid Vector position";
    }

    ++parent_iter; // get next parent
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void IsoVector::buildDecayMatrix() {
  double decayConst = 0; // decay constant, in inverse years
  int jcol = 1;
  int n = parent_.size();
  decayMatrix_ = Matrix(n,n);

  ParentMap::const_iterator parent_iter = parent_.begin(); // get first parent

  // populates the decay matrix column by column
  while( parent_iter != parent_.end() ) {
    jcol = parent_iter->second.first; // determines column index
    decayConst = parent_iter->second.second;
    decayMatrix_(jcol,jcol) = -1 * decayConst; // sets A(i,i) value
 
    // processes the vector in the daughters map if it is not empty
    if ( !daughters_.find(jcol)->second.empty() ) {
      // an iterator that points to 1st daughter in the vector
      // pair<isotope,branchratio>
      vector< pair<int, double> >::const_iterator
        iso_iter = daughters_.find(jcol)->second.begin();

      // processes all daughters of the parent
      while ( iso_iter != daughters_.find(jcol)->second.end() ) {
        int iso = iso_iter->first;
        int irow = parent_.find(iso)->second.first; // determines row index
        double branchRatio = iso_iter->second;
        decayMatrix_(irow,jcol) = branchRatio * decayConst; // sets A(i,j) value
    
        ++iso_iter; // get next daughter
      }
    }
    ++parent_iter; // get next parent
  }
}

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
    throw CycRangeException("Invalid isotope identifier.");
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool IsoVector::isZero(int tope) {
  validateIsotopeNumber(tope);

  int grams_per_kg = 1000;
  double atoms_eps = EPS_KG * grams_per_kg / MT->getMassInGrams(tope) ; 

  if (atom_comp_.count(tope) == 0) {
    return true;
  }
  return (fabs(atom_comp_[tope]) < atoms_eps);
}

