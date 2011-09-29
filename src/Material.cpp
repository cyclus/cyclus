// Material.cpp
#include <iostream>
#include <fstream>
#include <vector>

#include "Material.h"
#include "BookKeeper.h"
#include "GenException.h"
#include "MassTable.h"
#include "Logician.h"
#include "Timer.h"
#include "UniformTaylor.h"

using namespace std;

// Static variables to be initialized.
ParentMap Material::parent_ = ParentMap();
DaughtersMap Material::daughters_ = DaughtersMap();
Matrix Material::decayMatrix_ = Matrix();
int Material::nextID_ = 0;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
Material::Material(): atomEqualsMass_(true), total_mass_(0), total_atoms_(0) {
  ID_ = nextID_++;
  facHist_ = FacHistory() ;
  CompMap zero_map;
  zero_map.insert(make_pair(Iso(92235),0));
  massHist_.insert(make_pair(TI->getTime(), zero_map));
  compHist_.insert(make_pair(TI->getTime(), zero_map));
  BI->registerMatChange(this);
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
Material::Material(xmlNodePtr cur) {
  ID_ = nextID_++;
  
  recipeName_ = XMLinput->get_xpath_content(cur,"name");

  string comp_type = XMLinput->get_xpath_content(cur,"basis");
  CompMap &comp_map = ( "atom" != comp_type ? massHist_[TI->getTime()] : 
                       compHist_[TI->getTime()]);
  double &total_comp = ( "atom" != comp_type ? total_mass_ : total_atoms_);

  units_ = XMLinput->get_xpath_content(cur,"unit");
  
  total_comp = atoi(XMLinput->get_xpath_content(cur,"total"));

  xmlNodeSetPtr isotopes = XMLinput->get_xpath_elements(cur,"isotope");

  for (int i=0;i<isotopes->nodeNr;i++)
  {
    xmlNodePtr iso_node = isotopes->nodeTab[i];
    Iso isotope = atoi(XMLinput->get_xpath_content(iso_node,"id"));
    comp_map[isotope] = atof(XMLinput->get_xpath_content(iso_node,"comp"));
  }
  
  if ( "atom" != comp_type)
    rationalize_M2A();
  else
    rationalize_A2M();

  facHist_ = FacHistory() ;
  BI->registerMatChange(this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
Material::Material(CompMap comp, string mat_unit, string rec_name, double size, Basis type) {
  
  ID_=nextID_++;

  units_ = mat_unit;
  recipeName_ = rec_name;

  CompMap &comp_map = ( ATOMBASED != type ? massHist_[TI->getTime()] : 
                       compHist_[TI->getTime()]);
  double &total_comp = ( ATOMBASED != type ? total_mass_ : total_atoms_);

  total_comp = size;
  comp_map = comp;

  if ( MASSBASED == type)
    rationalize_M2A();
  else if (ATOMBASED == type)
    rationalize_A2M();
  else 
    throw GenException("Type options are currently MASSBASED or ATOMBASED !");

  facHist_ = FacHistory() ;
  BI->registerMatChange(this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void Material::load_recipes() {

  /// load recipes from file
  xmlNodeSetPtr nodes = XMLinput->get_xpath_elements("/*/recipe");
  
  for (int i=0;i<nodes->nodeNr;i++) 
    LI->addRecipe(XMLinput->getCurNS() + XMLinput->get_xpath_content(nodes->nodeTab[i], "name"),
                  new Material(nodes->nodeTab[i]));

  /// load recipes from databases
  nodes = XMLinput->get_xpath_elements("/*/recipebook");

  for (int i=0;i<nodes->nodeNr;i++)
  load_recipebook(XMLinput->get_xpath_content(nodes->nodeTab[i], "filename"),
                  XMLinput->get_xpath_content(nodes->nodeTab[i], "namespace"),
                  XMLinput->get_xpath_content(nodes->nodeTab[i], "format"));

}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void Material::load_recipebook(string filename, string ns, string format) {
  XMLinput->extendCurNS(ns);

  if ("xml" == format)
    XMLinput->load_recipebook(filename);
  else
    throw GenException(format + "is not a supported recipebook format.");

  XMLinput->stripCurNS();
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const bool Material::isNeg(Iso tope) const {
  if (this->getAtomComp(tope) == 0) return false;

  // (kg) * (g/kg) * (mol/g)
  Atoms atoms_eps =  eps * 1e3 / Material::getAtomicMass(tope); 
  return (this->getAtomComp(tope) + atoms_eps < 0);
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const bool Material::isZero(Iso tope) const {
  // (kg) * (g/kg) * (mol/g) 
  Atoms atoms_eps = eps * 1e3 / Material::getAtomicMass(tope) ; 
  return (fabs(this->getAtomComp(tope)) < atoms_eps);
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const double Material::getIsoMass(Iso tope) const {
  map<Iso, Atoms> currComp = this->getAtomComp();
  return total_mass_*Material::getIsoMass(tope, currComp);
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double Material::getIsoMass(Iso tope, const CompMap& comp) {
  // If the given isotope is present, calculate and return its mass. 
  // Else return 0.

  CompMap::const_iterator searchIso = comp.find(tope);
  double massToRet = 0;

  if (searchIso != comp.end()) 
    massToRet = (*searchIso).second*Material::getAtomicMass(tope)/1e3;
  return massToRet;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Mass Material::getAtomicMass(Iso tope) {
  Mass toRet = MT->getMass(tope);
  return toRet;
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const double Material::getEltMass(int elt) const {
  map<Iso, Atoms> currComp = this->getAtomComp();
  return total_mass_*Material::getEltMass(elt, currComp);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double Material::getEltMass(int elt, const map<Iso, Atoms>& comp) {
  // Iterate through the current composition...
  map<Iso, Atoms>::const_iterator iter = comp.begin();
  double massToRet = 0;

  while (iter != comp.end()) {

    // ...get each isotope and add to the mass tally if the isotope is of the
    // given element.

    int itAN = Material::getAtomicNum(iter->first);
    if (itAN == elt) 
      massToRet = massToRet + getIsoMass(iter->first, comp);
    iter ++;
  }

  return massToRet;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double Material::getTotMass(const CompMap& comp) {
  // Sum the masses of the isotopes.
  CompMap::const_iterator iter = comp.begin();
  double massToRet = 0;

  while (iter != comp.end()) {
    massToRet = massToRet + Material::getIsoMass(iter->first, comp);
    iter ++;
  }
  return massToRet;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double Material::getTotAtoms(const CompMap& comp) {
  // Sum the atoms of the isotopes.
  CompMap::const_iterator iter = comp.begin();
  double atoms = 0;

  while (iter != comp.end()) {
    atoms = atoms + Material::getAtomComp(iter->first, comp);
    iter ++;
  }
  return atoms;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Material::changeComp(Iso tope, Atoms change, int time) {
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
    throw GenException("Tried to make isotope composition negative.");
  }

  total_atoms_ += change;

  rationalize_A2M();

  BI->registerMatChange(this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Material::changeAtomComp(CompMap newComp, int time) {
  // To replace the whole composition, we first erase that entry
  compHist_.erase(time);
  // then we insert the new composition
  compHist_.insert(make_pair(time, newComp));
  rationalize_A2M();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Material::changeMassComp(CompMap newComp, int time) {
  // To replace the whole composition, we first erase that entry
  massHist_.erase(time);
  // then we insert the new composition
  massHist_.insert(make_pair(time, newComp));
  rationalize_M2A();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const CompMap Material::getMassComp() const {
  CompMap comp;
  MassHistory::const_reverse_iterator it = massHist_.rbegin();
  if (it != massHist_.rend()) {
    comp = it->second;
  } else {
    comp.insert(make_pair(Iso(92235),Atoms(0)));
  }
  return comp;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const Mass Material::getMassComp(Iso tope) const {
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
double Material::getMassComp(Iso tope, const CompMap& comp) {
  // If the given isotope is present, calculate and return its comp. 
  // Else return 0.

  CompMap::const_iterator searchIso = comp.find(tope);
  double massToRet = 0;
  if (searchIso != comp.end()) {
    massToRet = (*searchIso).second;
  }
  return massToRet;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const CompMap Material::getAtomComp() const {
  CompMap comp;
  CompHistory::const_reverse_iterator it = compHist_.rbegin();
  if (it != compHist_.rend()){
    comp = it->second;
  } else {
    comp.insert(make_pair(Iso(92235),Atoms(0)));
  }
  return comp;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double Material::getAtomComp(Iso tope, const CompMap& comp) {
  // If the given isotope is present, calculate and return its comp. 
  // Else return 0.

  CompMap::const_iterator searchIso = comp.find(tope);
  double atoms = 0;
  if (searchIso != comp.end()) {
    atoms = (*searchIso).second;
  }
  return atoms;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const CompMap Material::getFracComp(double frac) const {
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
const Atoms Material::getAtomComp(Iso tope) const {
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
void Material::absorb(Material* matToAdd) {
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
void Material::extract(Material* matToRem) {
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
int Material::getAtomicNum(Iso tope) {
  // Make sure the number's in a reasonable range.
  if (isAtomicNumValid(tope)) {
    throw GenException("Tried to get atomic number of invalid isotope");
  }

  // Get the atomic number and return.
  return tope / 1000; // integer division
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int Material::getMassNum(Iso tope) {
  // Make sure the number's in a reasonable range.
  if (isAtomicNumValid(tope)) {
    throw GenException("Tried to get atomic number of invalid isotope");
  }

  return tope % 1000;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool Material::isAtomicNumValid(Iso tope) {
  int lower_limit = 1001;
  int upper_limit = 1182949;

  if (tope < lower_limit || tope > upper_limit) {
    return false;
  } else {
    return true;
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Material* Material::extractMass(Mass amt) {
  CompMap comp = this->getMassComp();
  Material* newMat = new Material(comp , units_, " ", amt, MASSBASED);
  this->extract(newMat);
  return newMat;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void Material::normalize(CompMap &comp_map) {
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
void Material::rationalize_A2M() {
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
void Material::rationalize_M2A() {
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
void Material::print() {
    printComp("Atom composition:", compHist_[TI->getTime()]);
    cout << "\tTotal atoms: " << this->getTotAtoms() 
        << " moles per " << units_ << endl;
    printComp("Mass composition:", massHist_[TI->getTime()]);
    cout << "\tTotal mass: " << this->getTotMass() 
        << " kg per " << units_ << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void Material::printComp(string header, CompMap comp_map) {

  cout << "\t" << header << endl;
  for (CompMap::iterator iso = comp_map.begin();
       iso != comp_map.end();
       iso++) {
    cout << "\t" << (*iso).first << " : " <<  (*iso).second << endl;
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Material::loadDecayInfo() {
  ifstream decayInfo ("Data/decayInfo.dat");

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
      throw GenException(err_msg);
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
              throw GenException(string("A duplicate daughter isotope, %i , was found in decayInfo.dat", iso));
            } 
          }
          temp[i] = make_pair(iso, branchRatio);
        }
           
        daughters_[jcol] = temp;
        ++jcol; // set next column
      } else {
        string err_msg;
        err_msg = "A duplicate parent isotope was found in 'decayInfo.dat'";
        throw GenException(err_msg);
      }
      decayInfo >> iso; // get next parent
    } 
    // builds the decay matrix from the parent and daughter maps
    makeDecayMatrix();
  } else {
    throw GenException("The file 'decayInfo.dat' does not exist");
  }
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Material::makeDecayMatrix() {
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
Vector Material::makeCompVector() const {
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
map<Iso, Atoms> Material::makeCompMap(const Vector & compVector) {
  map<Iso, Atoms> compMap; // new composition map
  ParentMap::const_iterator parent_iter = parent_.begin(); // get first parent

  // loops through the ParentMap and populates the new composition map with
  // the number density from the compVector parameter for each isotope
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
      throw GenException("Decay Error - invalid Vector position");
    }

    ++parent_iter; // get next parent
  }

  return compMap;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Material::decay(double months) {
  try {
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
  catch ( string e ) {
    throw GenException(e);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Material::decay() {
        
  // Figure out the time this object was most recently updated at.
  int t0 = compHist_.rbegin()->first;

  // Figure out what time it is now.
  int tf = Timer::Instance()->getTime();
  this->decay(tf - t0);
}

