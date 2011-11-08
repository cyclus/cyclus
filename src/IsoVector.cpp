// Material.cpp
#include "Material.h"

#include "BookKeeper.h"
#include "CycException.h"
#include "MassTable.h"
#include "Logician.h"
#include "Timer.h"
#include "Env.h"
#include "UniformTaylor.h"
#include "InputXML.h"
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

  total_mass_ = 0;
  total_atoms_ = 0;
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
IsoVector::IsoVector(CompMap comp, std::string mat_unit, std::string rec_name, double size, Basis type) {
  
  ID_=nextID_++;

  units_ = mat_unit;
  recipeName_ = rec_name;

  atom_comp_ = comp;
  total_atoms_ = size;
  if ( MASSBASED == type) {
    total_mass_ = size;
    rationalize_M2A();
  } else if (ATOMBASED == type) {
  } else {
    throw CycRangeException("Type options are currently MASSBASED or ATOMBASED !");
  }
}

IsoVector::operator+(IsoVector right_side) {

  IsoVector sum;
  for comp in 

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
    makeDecayMatrix();
  } else {
    throw CycIOException("Could not find file 'decayInfo.dat'.");
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const double IsoVector::mass(int tope) const {
  if (atom_comp_.count(tope) == 0) {
    return 0;
  } else {
    double grams_per_atom;
    int grams_per_kg = 1000;
    grams_per_atom = MT->getMassInGrams(isotope);

    return atom_comp_[tope] * grams_per_atom / grams_per_kg;
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const double IsoVector::atomCount(int tope) const {
  // If the isotope isn't currently present, return 0. Else return the 
  // isotope's current number density.
  if (atom_comp_.count(tope) == 0) {
    return 0;
  } else {
    return atom_comp_[tope] * total_atoms_;
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const double IsoVector::getEltMass(int elt) const {

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
void IsoVector::decay(double time_change) {
  // gets the initial composition Vector N_o for this Material object
  Vector N_o = this->makeCompVector();

  // convert months to years to match decay constant units in matrix
  double years = time_change / 12;

  // solves the decay equation for the final composition Vector N_t using a
  // matrix exponential method
  Vector N_t = UniformTaylor::matrixExpSolver(decayMatrix_, N_o, years);

  // converts the Vector solution N_t into a composition map
  map<int, double> newComp = makeCompMap(N_t);

  // assigns the new composition map to this Material object
  int time = Timer::Instance()->getTime();
  this->changeAtomComp(newComp,time);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const bool IsoVector::isNeg(int tope) const {
  double num_atoms = getAtomComp(tope);

  if (num_atoms == 0) {
    return false;
  } else {
    int grams_per_kg = 1000;
    double atoms_eps =  EPS_KG * grams_per_kg / MT->getMassInGrams(tope); 
    return (num_atoms + atoms_eps < 0);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const bool IsoVector::isZero(int tope) const {
  int grams_per_kg = 1000;
  double atoms_eps = EPS_KG * grams_per_kg / MT->getMassInGrams(tope) ; 
  return (fabs(getAtomComp(tope)) < atoms_eps);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void IsoVector::validateAtomicNumber(int tope) {
  int lower_limit = 1001;
  int upper_limit = 1182949;

  if (tope < lower_limit || tope > upper_limit) {
    throw CycRangeException("Tried to get atomic number of invalid isotope");
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void IsoVector::normalize(CompMap &comp_map) {
  double sum_total_comp = 0;
  CompMap::iterator entry;
  for (entry = comp_map.begin(); entry != comp_map.end(); entry++) {
    sum_total_comp += (*entry).second;
  }

  for (entry = comp_map.begin(); entry != comp_map.end(); entry++) {
    (*entry).second /= sum_total_comp;
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
CompMap atomComp() {
  return atom_comp_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void IsoVector::rationalize_M2A() {
  int isotope;
  double mass_kg, grams_per_atom;
  int grams_per_kg = 1000;

  normalize(atom_comp_);

  total_atoms_ = 0;
  for(CompMap::iterator entry = atom_comp_.begin();
      entry != atom_comp_.end();
      entry++) {
    isotope = (*entry).first;
    mass_kg = (*entry).second;
    grams_per_atom = MT->getMassInGrams(isotope);

    atom_comp_[isotope] = mass_kg * grams_per_kg / grams_per_atom;
    total_atoms_ += atom_comp_[isotope];
  }
  total_atoms_ *= total_mass;
  
  normalize(atom_comp_);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Vector IsoVector::makeCompVector() const {
  // gets the current composition map of the Material object
  map<int, double> compMap = this->getAtomComp();
  map<int, double>::const_iterator comp_iter = compMap.begin();
        
  Vector compVector = Vector(parent_.size(),1);

  // loops through the composition map and populates the Vector with the
  // number density of each isotope
  while( comp_iter != compMap.end() ) {
    int iso = comp_iter->first;
    long double numDens = comp_iter->second;

    // if the isotope is tracked in the decay matrix  
    if ( parent_.find(iso) != parent_.end() ) {
      int col = parent_.find(iso)->second.first; // get Vector position
      compVector(col,1) = numDens;
    // if it is not in the decay matrix, then it is added as a stable isotope
    } else {
      double decayConst = 0;
      int col = parent_.size() + 1;
      parent_[iso] = make_pair(col, decayConst);  // add isotope to parent map

      int nDaughters = 0;
      vector< pair<int,double> > temp(nDaughters);
      daughters_[col] = temp;  // add isotope to daughters map
      
      vector<long double> row(1,numDens);
      compVector.addRow(row);  // add isotope to the end of the Vector
      
      makeDecayMatrix();  // recreate the decay matrix with new stable isotope
    }

    ++comp_iter; // get next isotope
  }

  return compVector;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void IsoVector::makeFromVect(const Vector & compVector) {
  map<int, double> compMap; // new composition map
  ParentMap::const_iterator parent_iter = parent_.begin(); // get first parent

  // loops through the ParentMap and populates the new composition map with
  // the number density from the compVector parameter for each isotope
  for (int i = 0; i < compVector.numRows(); i++) {
    if ( numDens != 0 ) {
      compMap.insert( make_pair(iso, numDens) );
    }
  }
  while( parent_iter != parent_.end() ) {
    int iso = parent_iter->first;
    int col = parent_.find(iso)->second.first; // get Vector position
    
    // checks to see if the Vector position is valid
    if ( col <= compVector.numRows() ) {
      double numDens = compVector(col,1);
      // adds isotope to the map if its number density is non-zero
      if ( numDens != 0 )
        compMap.insert( make_pair(iso, numDens) );
    } else {
      throw CycRangeException("Decay Error - invalid Vector position");
    }

    ++parent_iter; // get next parent
  }

  return compMap;
}

