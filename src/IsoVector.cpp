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
  total_mass_ = 0;
  total_atoms_ = 0;

  ID_ = nextID_++;
  CompMap zero_map;
  zero_map.insert(make_pair(Iso(92235),0));
  massHist_.insert(make_pair(TI->getTime(), zero_map));
  compHist_.insert(make_pair(TI->getTime(), zero_map));
  BI->registerMatChange(this);
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
IsoVector::IsoVector(CompMap comp, std::string mat_unit, std::string rec_name, double size, Basis type) {
  
  ID_=nextID_++;

  units_ = mat_unit;
  recipeName_ = rec_name;

  comp_map_ = comp;

  if ( MASSBASED == type) {
    total_mass_ = size;
    rationalize_M2A();
  } else if (ATOMBASED == type) {
    total_atoms_ = size;
    rationalize_A2M();
  } else {
    throw CycRangeException("Type options are currently MASSBASED or ATOMBASED !");
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const bool IsoVector::isNeg(Iso tope) const {
  if (this->getAtomComp(tope) == 0) return false;

  // (kg) * (g/kg) * (mol/g)
  Atoms atoms_eps =  EPS * 1e3 / IsoVector::getAtomicMass(tope); 
  return (this->getAtomComp(tope) + atoms_eps < 0);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const bool IsoVector::isZero(Iso tope) const {
  // (kg) * (g/kg) * (mol/g) 
  Atoms atoms_eps = EPS * 1e3 / IsoVector::getAtomicMass(tope) ; 
  return (fabs(this->getAtomComp(tope)) < atoms_eps);
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const double IsoVector::getIsoMass(Iso tope) const {
  map<Iso, Atoms> currComp = this->getAtomComp();
  return total_mass_*IsoVector::getIsoMass(tope, currComp);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Mass IsoVector::getAtomicMass(Iso tope) {
  Mass toRet = MT->getMass(tope);
  return toRet;
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const double IsoVector::getEltMass(int elt) const {
  map<Iso, Atoms> currComp = this->getAtomComp();
  return total_mass_*IsoVector::getEltMass(elt, currComp);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void IsoVector::changeComp(Iso tope, Atoms change, int time) {
  // If the composition has already changed during this timestep, we copy and 
  // delete the current entry, modify it, and re-add. If there's no entry 
  // for this timestep, simply copy the most recent one, edit, and add.
  CompMap newComp;
  if (compHist_.end() != compHist_.find(time)) {
    newComp = CompMap(compHist_[time]);
    compHist_.erase(time);
  } else {
    newComp = CompMap((*compHist_.rbegin()).second);
  }

  // We need to normalize this number of atoms to the current recipe
  double newVal = change;

  if (total_atoms_ != 0) {
    newVal = newVal/total_atoms_;
  }

  // If the isotope's already in the vector, add to the amount
  if (newComp.end() != newComp.find(tope)) {
    double oldVal = newComp[tope];
    newComp.erase(tope);
    newComp.insert(make_pair(tope, oldVal + newVal));
  // otherwise add a new entry.
  } else {
    newComp.insert(make_pair(tope, newVal));
  }

  // Now insert the copy for the current time.
  compHist_.insert(make_pair(time, newComp));

  // If there's no material of the given isotope left (w/r/t COM tolerance), 
  // set the comp to zero.
  if (this->isZero(tope)) {
    CompMap newComp = compHist_[time];
    newComp.erase(tope);
    compHist_.erase(time);
    compHist_.insert(make_pair(time, newComp));
  }

  // If the value is negative, throw an exception;
  // something's gone wrong.
  if (this->isNeg(tope)) {
    throw CycRangeException("Tried to make isotope composition negative.");
  }

  total_atoms_ += change;

  rationalize_A2M();

  BI->registerMatChange(this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const Mass IsoVector::getMassComp(Iso tope) const {
  CompMap currComp = this->getMassComp();
  // If the isotope isn't currently present, return 0. Else return the 
  // isotope's current number density.
  if (currComp.find(tope) == currComp.end()) {
    return 0;
  } else {
    return currComp[tope];
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const CompMap IsoVector::getFracComp(double frac) const {
  // Create a new composition object.
  CompMap newComp;

  // Iterate through the current composition vector and add to the new object 
  // the specified fraction of each isotope.
  CompMap currComp = this->getAtomComp();
  CompMap::iterator iter = currComp.begin();

  while (iter != currComp.end()) {
    newComp[iter->first] = iter->second * frac;
    iter ++;
  }

  return newComp;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const Atoms IsoVector::getAtomComp(Iso tope) const {
  CompMap currComp = this->getAtomComp();

  // If the isotope isn't currently present, return 0. Else return the 
  // isotope's current number density.
  if (currComp.find(tope) == currComp.end()) {
    return 0;
  } else {
    return currComp[tope];
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void IsoVector::absorb(Material* matToAdd) {
  // Get the given Material's composition.
  CompMap compToAdd = matToAdd->getAtomComp();

  // Iterate over the isotopes in the Material we're adding and add them to 
  // this Material.
  CompMap::const_iterator iter = compToAdd.begin();
  Iso isoToAdd;
  Atoms atomsToAdd;

  while (*iter != *(compToAdd.end())) {
    isoToAdd = iter->first;
    atomsToAdd = matToAdd->getTotAtoms() * iter->second;
    this->changeComp(isoToAdd, atomsToAdd, TI->getTime());
    iter++;
  }

  // Delete the given Material.
  delete matToAdd;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void IsoVector::extract(Material* matToRem) {
  // Get the given Material's composition.
  CompMap compToRem = matToRem->getAtomComp();

  // Iterate over the isotopes in the Material we're removing and subtract 
  // them from this Material.
  CompMap::iterator iter = compToRem.begin();
  Iso isoToRem;
  Atoms aToRem;

  while (*iter != *(compToRem.end())) {
    isoToRem = iter->first;
    aToRem = 0 - matToRem->getTotAtoms() * iter->second;
    this->changeComp(isoToRem, aToRem, TI->getTime());
    iter++;
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int IsoVector::getAtomicNum(Iso tope) {
  // Make sure the number's in a reasonable range.
  if (isAtomicNumValid(tope)) {
    throw CycRangeException("Tried to get atomic number of invalid isotope");
  }

  // Get the atomic number and return.
  return tope / 1000; // integer division
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int IsoVector::getMassNum(Iso tope) {
  // Make sure the number's in a reasonable range.
  if (isAtomicNumValid(tope)) {
    throw CycRangeException("Tried to get atomic number of invalid isotope");
  }

  return tope % 1000;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool IsoVector::isAtomicNumValid(Iso tope) {
  int lower_limit = 1001;
  int upper_limit = 1182949;

  if (tope < lower_limit || tope > upper_limit) {
    return false;
  } else {
    return true;
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Material* IsoVector::extractMass(Mass amt) {
  CompMap comp = this->getMassComp();
  Material* newMat = new Material(comp , units_, " ", amt, MASSBASED);
  this->extract(newMat);
  return newMat;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void IsoVector::normalize(CompMap &comp_map) {
  double sum_total_comp = 0;
  CompMap::iterator entry;
  for (entry = comp_map.begin(); entry != comp_map.end(); entry++) {
    //if (this->isZero((*entry).first))
    //  comp_map.erase((*entry).first);
    //else
    sum_total_comp += (*entry).second;
  }

  for (entry = comp_map.begin(); entry != comp_map.end(); entry++) {
    (*entry).second /= sum_total_comp;
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void IsoVector::rationalize_A2M() {
  normalize(compHist_[TI->getTime()]);

  total_atoms_ = this->getTotAtoms();
  total_mass_ = 0;

  // loop through each isotope in the composition for the current time.
  for(CompMap::iterator entry = compHist_[TI->getTime()].begin();
      entry != compHist_[TI->getTime()].end();
      entry++) {
    // multiply the number of atoms by the mass number of that isotope and convert to kg
    massHist_[TI->getTime()][(*entry).first] = 
              (*entry).second*getAtomicMass((double)(*entry).first)/1e3;
    total_mass_ += total_atoms_ * (*entry).second * 
              getAtomicMass((double)(*entry).first)/1e3;
  }

  normalize(massHist_[TI->getTime()]);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void IsoVector::rationalize_M2A() {
  normalize(massHist_[TI->getTime()]);

  total_mass_ = this->getTotMass();
  total_atoms_ = 0;

  for(CompMap::iterator entry = massHist_[TI->getTime()].begin();
      entry != massHist_[TI->getTime()].end();
      entry++) {
    compHist_[TI->getTime()][(*entry).first] = 
                      (*entry).second*1e3/getAtomicMass((*entry).first);
    total_atoms_ += total_mass_ * (*entry).second * 1e3 / 
                      getAtomicMass((*entry).first);
  }
  
  normalize(compHist_[TI->getTime()]);
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
void IsoVector::makeDecayMatrix() {
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
Vector IsoVector::makeCompVector() const {
  // gets the current composition map of the Material object
  map<Iso, Atoms> compMap = this->getAtomComp();
  map<Iso, Atoms>::const_iterator comp_iter = compMap.begin();
        
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
  map<Iso, Atoms> compMap; // new composition map
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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void IsoVector::decay(double months) {
  // gets the initial composition Vector N_o for this Material object
  Vector N_o = this->makeCompVector();

  // convert months to years to match decay constant units in matrix
  double years = months / 12;

  // solves the decay equation for the final composition Vector N_t using a
  // matrix exponential method
  Vector N_t = UniformTaylor::matrixExpSolver(decayMatrix_, N_o, years);

  // converts the Vector solution N_t into a composition map
  map<Iso, Atoms> newComp = makeCompMap(N_t);

  // assigns the new composition map to this Material object
  int time = Timer::Instance()->getTime();
  this->changeAtomComp(newComp,time);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void IsoVector::decay() {
        
  // Figure out the time this object was most recently updated at.
  int t0 = compHist_.rbegin()->first;

  // Figure out what time it is now.
  int tf = Timer::Instance()->getTime();
  this->decay(tf - t0);
}

