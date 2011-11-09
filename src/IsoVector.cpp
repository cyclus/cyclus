// IsoVector.cpp
#include "IsoVector.h"

#include "CycException.h"
#include "MassTable.h"
#include "Env.h"
#include "UniformTaylor.h"
#include "Logger.h"

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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
IsoVector::IsoVector() {
  ID_ = nextID_++;
  total_atoms_ = 0;
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
IsoVector::IsoVector(CompMap initial_comp) {
  ID_ = nextID_++;
  total_atoms_ = 0;
  atom_comp_ = initial_comp;

  validateComposition();
};

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
    diff_comp[isotope] -= rhs_atoms;
  }

  IsoVector temp(diff_comp);
  return (temp);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int IsoVector::getAtomicNum(int tope) {
  validateAtomicNumber(tope);
  return tope / 1000; // integer division
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int IsoVector::getMassNum(int tope) {
  validateAtomicNumber(tope);
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
  return 0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double IsoVector::mass(int tope) {
  if (atom_comp_.count(tope) == 0) {
    return 0;
  } else {
    double grams_per_atom;
    int grams_per_kg = 1000;
    grams_per_atom = MT->getMassInGrams(tope);

    return atom_comp_[tope] * grams_per_atom / grams_per_kg;
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double IsoVector::atomCount(int tope) {
  // If the isotope isn't currently present, return 0. Else return the 
  // isotope's current number density.
  if (atom_comp_.count(tope) == 0) {
    return 0;
  } else {
    return atom_comp_[tope];
  }
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
      // makes an iterator that points to the first daughter in the vector
      vector< pair<Iso,BranchRatio> >::const_iterator
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

    validateAtomicNumber(tope);

    atoms_eps =  EPS_KG * grams_per_kg / MT->getMassInGrams(tope); 
    if (num_atoms < -1 * atoms_eps) {
      string err_msg = "Composition has negative quantity for an isotope.";
      throw CycRangeException(err_msg);
    }
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void IsoVector::validateAtomicNumber(int tope) {
  int lower_limit = 1001;
  int upper_limit = 1182949;

  if (tope < lower_limit || tope > upper_limit) {
    throw CycRangeException("Invalid isotope identifier.");
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool IsoVector::isZero(int tope) {
  int grams_per_kg = 1000;
  double atoms_eps = EPS_KG * grams_per_kg / MT->getMassInGrams(tope) ; 

  if (atom_comp_.count(tope) == 0) {
    return 0;
  }
  return (fabs(atom_comp_[tope]) < atoms_eps);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
CompMap IsoVector::normalized() {
  CompMap normalized_comp;
  CompMap::iterator entry;

  for (entry = atom_comp_.begin(); entry != atom_comp_.end(); entry++) {
    normalized_comp[entry->first] = entry->second / total_atoms_;
  }
  return normalized_comp;
}

