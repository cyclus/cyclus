// Material.cpp
// Implements the Material class
#include "Material.h"
#include "Timer.h"
#include <math.h>
#include <sstream>
#include <fstream>
#include "GenException.h"
#include "UniformTaylor.h"

// Static variables to be initialized.
ParentMap Material::parent = ParentMap();
DaughtersMap Material::daughters = DaughtersMap();
Matrix Material::decayMatrix = Matrix();
map<Spectrum, map<Iso, double> > Material::avgCapXSects = 
  map<Spectrum, map<Iso, double> >();
map<Spectrum, map<Iso, double> > Material::avgFisXSects = 
  map<Spectrum, map<Iso, double> >();
map<Spectrum, map<Iso, double> > Material::avgNus = 
  map<Spectrum, map<Iso, double> >();

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Material::Material() 
{
  throw GenException("Don't use default Material constructor!");
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Material::Material(map<Iso, NumDens> comp, ChemForm form, Commodity* commod) 
  : myForm(form), myType(commod)
{
  compHist[TI->getTime()] = comp;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Material::~Material()
{
  // Only write history if this Material has gone somewhere. Otherwise, it was 
  // probably a Material created solely for book-keeping or calculational 
  // purposes.
  if (!facHist.empty()) {
    writeHist();
  }
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Material::changeComp(Iso tope, NumDens change, int time)
{

  // If the composition has already changed during this timestep, we copy and 
  // delete the current entry, modify it, and re-add. If there's no entry 
  // for this timestep, simply copy the most recent one, edit, and add.
  map<Iso, NumDens> newComp;
  if (compHist.end() != compHist.find(time)) {
    newComp = map<Iso, NumDens>(compHist[time]);
    compHist.erase(time);
  }
  else
    newComp = map<Iso, NumDens>((*compHist.rbegin()).second);

  // If the isotope's already in the vector, add to the amount, otherwise
  // add a new entry.
  if (newComp.end() != newComp.find(tope)) {
    double oldVal = newComp[tope];
    newComp.erase(tope);
    newComp.insert(make_pair(tope, oldVal + change));
  }
  else
    newComp.insert(make_pair(tope, change));

  // Now insert the copy for the current time.
  compHist.insert(make_pair(time, newComp));

  // If there's no material of the given isotope left (w/r/t COM tolerance), 
  // set the nd to zero. If the value is negative, throw an exception;
  // something's gone wrong.
  if (this->isZero(tope)) {

    map<Iso, NumDens> newComp = compHist[time];
    newComp.erase(tope);
    newComp.insert(make_pair(tope, 0));
    compHist.insert(make_pair(time, newComp));
  }

  if (this->isNeg(tope))
    throw GenException("Tried to make isotope composition negative.");

}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Material::changeComp(map<Iso, NumDens> newComp, int time)
{
  compHist[time] = newComp;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const map<Iso, NumDens> Material::getComp() const
{
	// Assign a reverse iterator for the compHist map.
  CompHistory::const_reverse_iterator it = compHist.rbegin();
	// Return the second to the last entry.
	//  (the last entry is a length)
  return it->second;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const NumDens Material::getComp(Iso tope) const
{
  map<Iso, NumDens> currComp = this->getComp();

  // If the isotope isn't currently present, return 0. Else return the 
  // isotope's current number density.
  if (currComp.find(tope) == currComp.end()) {
    return 0;
  }
  else
    return currComp[tope];
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const map<Iso, NumDens> Material::getFracComp(double frac) const
{
  // Create a new composition object.
  map<Iso, NumDens> newComp;

  // Iterate through the current composition vector and add to the new object 
  // the specified fraction of each isotope.
  map<Iso, NumDens> currComp = this->getComp();
  map<Iso, NumDens>::iterator iter = currComp.begin();

  while (iter != currComp.end()) {
    newComp[iter->first] = iter->second * frac;
    iter ++;
  }

  return newComp;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const double Material::getEltComp(int elt) const
{
  // Iterate through the current composition...
  map<Iso, NumDens> currComp = this->getComp();
  map<Iso, NumDens>::iterator iter = currComp.begin();
  NumDens nd = 0;

  while (iter != currComp.end()) {

    // ...get each isotope and add to the mass tally if the isotope is of the
    // given element.

    int itAN = Material::getAtomicNum(iter->first);
    if (itAN == elt) 
      nd = nd + this->getComp(iter->first);
    iter ++;
  }

  return nd;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const long Material::getSN() const
{
  return ID;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Commodity* Material::getCommod() const
{
  return myType;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Material::logTrans(int time, int fromFac, int toFac)
{
  facHist[time] = pair<int, int>(fromFac, toFac);

  // Also, make sure compHist as an entry for this time, since we need that to 
  // be true for writing to the database later.
  if (compHist.find(time) == compHist.end()) {
    map<Iso, NumDens> currComp = this->getComp();
    this->changeComp(currComp, time);
  }
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Material::writeHist()
{
//  map<int, long> compIDs = BI->writeMatCompHist(compHist, ID);
//  BI->writeMatFacHist(facHist, ID, compIDs);
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Material::absorb(Material* matToAdd)
{
  // Get the given Material's composition.
  map<Iso, NumDens> compToAdd = matToAdd->getComp();

  // Iterate over the isotopes in the Material we're adding and add them to 
  // this Material.
  map<Iso, NumDens>::iterator iter = compToAdd.begin();
  Iso isoToAdd;
  NumDens ndToAdd;

  while (*iter != *(compToAdd.end())) {
    isoToAdd = iter->first;
    ndToAdd = iter->second;
    this->changeComp(isoToAdd, ndToAdd, TI->getTime());
    iter ++;
  }

  // Delete the given Material.
  delete matToAdd;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Material::extract(Material* matToRem)
{
  // Get the given Material's composition.
  map<Iso, NumDens> compToRem = matToRem->getComp();

  // Iterate over the isotopes in the Material we're removing and subtract 
  // them from this Material.
  map<Iso, NumDens>::iterator iter = compToRem.begin();
  Iso isoToRem;
  NumDens ndToRem;

  while (*iter != *(compToRem.end())) {
    isoToRem = iter->first;
    ndToRem = 0 - iter->second;
    this->changeComp(isoToRem, ndToRem, TI->getTime());
    iter ++;
  }
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const double Material::getTotMass() const
{
  map<Iso, NumDens> comp = this->getComp();
  return Material::getTotMass(comp);
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const double Material::getTotNumDens() const
{
  map<Iso, NumDens> comp = this->getComp();
  return Material::getTotNumDens(comp);
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const double Material::getEltMass(int elt) const
{
  map<Iso, NumDens> currComp = this->getComp();
  return Material::getEltMass(elt, currComp);
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const double Material::getIsoMass(Iso tope) const
{
  map<Iso, NumDens> currComp = this->getComp();
  return Material::getIsoMass(tope, currComp);
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Material::changeCommod(Commodity* newCommod)
{
  if (newCommod == myType)
    throw GenException("Tried to change Commodity type to the current type.");

  if (newCommod->isFissile())
    throw GenException("Commodity type 'fissile' cannot be assigned to Materials");

  myType = newCommod;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Material::changeChemForm(ChemForm newForm)
{
  myForm = newForm;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int Material::getAtomicNum(Iso tope)
{
  // Let our dummy codes through.
  switch(tope) {
  case OTHER_FP :
  case OTHER_ACT :
  case OTHER_CS :
  case OTHER_KR :
  case OTHER_C :
  case OTHER_SR :
  case OTHER_I :
  case OTHER_TC :
    return tope / 10000; // integer division
  default :
    break;
  }

  // Make sure the number's in a reasonable range.
  if (tope < 10010 || tope > 1182949)
    throw GenException("Tried to get atomic number of invalid isotope");

  // Get the atomic number and return.
  return tope / 10000; // integer division
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int Material::getMassNum(Iso tope)
{
  // Let our dummy codes through.
  switch(tope) {
  case OTHER_FP :
  case OTHER_ACT :
  case OTHER_CS :
  case OTHER_KR :
  case OTHER_C :
  case OTHER_SR :
  case OTHER_I :
  case OTHER_TC :
    return (tope / 10) % 1000;
  default :
    break;
  }

  // Make sure the number's in a reasonable range.
  if (tope < 10010 || tope > 1182949)
    throw GenException("Tried to get atomic number of invalid isotope");

  // Get the mass number and return.
  return (tope / 10) % 1000;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double Material::getEltMass(int elt, const map<Iso, NumDens>& comp)
{
  // Iterate through the current composition...
  map<Iso, NumDens>::const_iterator iter = comp.begin();
  double mass = 0;

  while (iter != comp.end()) {

    // ...get each isotope and add to the mass tally if the isotope is of the
    // given element.

    int itAN = Material::getAtomicNum(iter->first);
    if (itAN == elt) 
      mass = mass + getIsoMass(iter->first, comp);
    iter ++;
  }

  return mass;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double Material::getIsoMass(Iso tope, const map<Iso, NumDens>& comp)
{
  // If the given isotope is present, calculate and return its mass. 
  // Else return 0.

  map<Iso, NumDens>::const_iterator searchIso = comp.find(tope);
  double mass = 0;
  if (searchIso != comp.end()) 
    mass = searchIso->second * Material::getMassNum(tope) / AV_NUM / 1e6;
  return mass;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
map<Iso, NumDens> Material::getFracComp(double frac, 
					const map<Iso, NumDens>& comp)
{
  // Create a new composition object.
  map<Iso, NumDens> newComp;

  // Iterate through the composition vector and add to the new object 
  // the specified fraction of each isotope.
  map<Iso, NumDens>::const_iterator iter = comp.begin();

  while (iter != comp.end()) {
    newComp[iter->first] = iter->second * frac;
    iter ++;
  }

  return newComp;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double Material::getTotMass(const map<Iso, NumDens>& comp)
{
  // Sum the masses of the isotopes.
  map<Iso, NumDens>::const_iterator iter = comp.begin();
  double mass = 0;

  while (iter != comp.end()) {
    mass = mass + Material::getIsoMass(iter->first, comp);
    iter ++;
  }
  return mass;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double Material::getTotNumDens(const map<Iso, NumDens>& comp)
{
  // Sum the number of atoms.
  map<Iso, NumDens>::const_iterator iter = comp.begin();
  NumDens nd = 0;

  while (iter != comp.end()) {
    nd += iter->second;
    iter ++;
  }
  return nd;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const bool Material::isNeg(Iso tope) const
{
  if (this->getComp(tope) == 0)
    return false;

  NumDens nd_eps = AV_NUM / Material::getMassNum(tope) * eps * 1e6; 
  return this->getComp(tope) + nd_eps < 0;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const bool Material::isZero(Iso tope) const
{
  NumDens nd_eps = AV_NUM / Material::getMassNum(tope) * eps * 1e6; 
  return fabs(this->getComp(tope)) < nd_eps;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool Material::containsActinides()
{
  int act = 89;
  int lawr = 103;

  // Check for actinides; return true if we've got some.
  for (int elt = act; elt <= lawr; elt++)
    if (this->getEltMass(elt) > eps)
      return true;

  // Else return false.
  return false;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool Material::containsFissionProducts()
{
  int zn = 30;
  int lu = 71;

  // Check for fission products; return true if we've got some.
  for (int elt = zn; elt <= lu; elt++)
    if (this->getEltMass(elt) > eps)
      return true;

  // Else return false.
  return false;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ChemForm Material::getForm() const
{
  return myForm;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Material* Material::extractMass(double mass)
{
  double frac = mass / this->getTotMass();
  map<Iso, NumDens> comp = this->getFracComp(frac);
  Material* newMat = new Material(comp , sol, myType);
  this->extract(newMat);
  return newMat;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
map<Iso, double> Material::convNumDensToMass(const map<Iso, NumDens>& 
					     compToConv)
{
  map<Iso, double> toRet = map<Iso, double>();
  map<Iso, NumDens>::const_iterator iter;
  for (iter = compToConv.begin(); iter != compToConv.end(); iter ++) 
    toRet[(*iter).first] = Material::getIsoMass((*iter).first, compToConv);

  return toRet;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Material::loadDecayInfo()
{
  ifstream decayInfo ("decayInfo.dat");

  if ( decayInfo.is_open() ) {
    int jcol = 1;
    int iso = 0;
    int nDaughters = 0;
    double decayConst = 0; // decay constant, in inverse years
    double branchRatio = 0;
      
    decayInfo >> iso;  // get first parent
    
    // checks to see if there are isotopes in 'decayInfo.dat'
    if ( decayInfo.eof() ) {
      throw GenException("There are no isotopes in the 'decayInfo.dat' file");
    }
    
    // processes 'decayInfo.dat'
    while ( !decayInfo.eof() ){
      // make parent
      decayInfo >> decayConst;
      decayInfo >> nDaughters;
     
      // checks for duplicate parent isotopes
      if ( parent.find(iso) == parent.end() ) {
        parent[iso] = make_pair(jcol, decayConst);
           
        // make daughters
        vector< pair<int,double> > temp(nDaughters);
   
        for ( int i = 0; i < nDaughters; ++i ) {
          decayInfo >> iso;
          decayInfo >> branchRatio;

          // checks for duplicate daughter isotopes
          for ( int j = 0; j < nDaughters; ++j ) {
            if ( temp[j].first == iso ) {
              throw GenException("A duplicate daughter isotope was found in decayInfo.dat");
            } 
          }

          temp[i] = make_pair(iso, branchRatio);
        }
           
        daughters[jcol] = temp;
        ++jcol; // set next column
      }
      else {
        throw GenException("A duplicate parent isotope was found in 'decayInfo.dat'");
      }
      decayInfo >> iso; // get next parent
    } 
    // builds the decay matrix from the parent and daughter maps
    makeDecayMatrix();
  }   
  else {
    throw GenException("The file 'decayInfo.dat' does not exist");
  }
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Material::makeDecayMatrix()
{
  double decayConst = 0; // decay constant, in inverse years
  int jcol = 1;
  int n = parent.size();
  decayMatrix = Matrix(n,n);

  ParentMap::const_iterator parent_iter = parent.begin(); // get first parent

  // populates the decay matrix column by column
  while( parent_iter != parent.end() ) {
    jcol = parent_iter->second.first; // determines column index
    decayConst = parent_iter->second.second;
    decayMatrix(jcol,jcol) = -1 * decayConst; // sets A(i,i) value
 
    // processes the vector in the daughters map if it is not empty
    if ( !daughters.find(jcol)->second.empty() ) {
      // makes an iterator that points to the first daughter in the vector
      vector< pair<Iso,BranchRatio> >::const_iterator
        iso_iter = daughters.find(jcol)->second.begin();

      // processes all daughters of the parent
      while ( iso_iter != daughters.find(jcol)->second.end() ) {
        int iso = iso_iter->first;
        int irow = parent.find(iso)->second.first; // determines row index
        double branchRatio = iso_iter->second;
        decayMatrix(irow,jcol) = branchRatio * decayConst; // sets A(i,j) value
    
        ++iso_iter; // get next daughter
      }
    }
    
    ++parent_iter; // get next parent
  }
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Vector Material::makeCompVector() const
{
  // gets the current composition map of the Material object
  map<Iso, NumDens> compMap = this->getComp();
  map<Iso, NumDens>::const_iterator comp_iter = compMap.begin();
        
  Vector compVector = Vector(parent.size(),1);

  // loops through the composition map and populates the Vector with the
  // number density of each isotope
  while( comp_iter != compMap.end() ) {
    int iso = comp_iter->first;
    long double numDens = comp_iter->second;

    // if the isotope is tracked in the decay matrix  
    if ( parent.find(iso) != parent.end() ) {
      int col = parent.find(iso)->second.first; // get Vector position
      compVector(col,1) = numDens;
    }
    // if it is not in the decay matrix, then it is added as a stable isotope
    else {
      double decayConst = 0;
      int col = parent.size() + 1;
      parent[iso] = make_pair(col, decayConst);  // add isotope to parent map

      int nDaughters = 0;
      vector< pair<int,double> > temp(nDaughters);
      daughters[col] = temp;  // add isotope to daughters map
      
      vector<long double> row(1,numDens);
      compVector.addRow(row);  // add isotope to the end of the Vector
      
      makeDecayMatrix();  // recreate the decay matrix with new stable isotope
    }

    ++comp_iter; // get next isotope
  }

  return compVector;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
map<Iso, NumDens> Material::makeCompMap(const Vector & compVector)
{
  map<Iso, NumDens> compMap; // new composition map
  ParentMap::const_iterator parent_iter = parent.begin(); // get first parent

  // loops through the ParentMap and populates the new composition map with
  // the number density from the compVector parameter for each isotope
  while( parent_iter != parent.end() ) {
    int iso = parent_iter->first;
    int col = parent.find(iso)->second.first; // get Vector position
    
    // checks to see if the Vector position is valid
    if ( col <= compVector.numRows() ) {
      double numDens = compVector(col,1);
      // adds isotope to the map if its number density is non-zero
      if ( numDens != 0 )
        compMap.insert( make_pair(iso, numDens) );
    }
    else {
      throw GenException("Decay Error - invalid Vector position");
    }

    ++parent_iter; // get next parent
  }

  return compMap;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Material::decay(double months)
{
  try {
    // gets the initial composition Vector N_o for this Material object
    Vector N_o = this->makeCompVector();

    // convert months to years to match decay constant units in matrix
    double years = months / 12;
  
    // solves the decay equation for the final composition Vector N_t using a
    // matrix exponential method
    Vector N_t = UniformTaylor::matrixExpSolver(decayMatrix, N_o, years);

    // converts the Vector solution N_t into a composition map
    map<Iso, NumDens> newComp = makeCompMap(N_t);
 
    // assigns the new composition map to this Material object
    int time = Timer::Instance()->getTime();
    this->changeComp(newComp,time);
  }
  catch ( string e ) {
    throw GenException(e);
  }
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Material::decay() {
        
  // Figure out the time this object was most recently updated at.
  int t0 = compHist.rbegin()->first;

  // Figure out what time it is now.
  int tf = Timer::Instance()->getTime();
  this->decay(tf - t0);
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Material::initAvgCapXSects(Spectrum s)
{
  map<Iso, double> spectCapXSects;

  switch(s) {

  case thermal :
    spectCapXSects[922340] = 17.19;
    spectCapXSects[922350] = 6.398;
    spectCapXSects[922360] = 8.487;
    spectCapXSects[922380] = 0.8718;
    spectCapXSects[932370] = 24.23;
    spectCapXSects[942380] = 15.26;
    spectCapXSects[942390] = 26.04;
    spectCapXSects[942400] = 43.88;
    spectCapXSects[942410] = 16.72;
    spectCapXSects[942420] = 25.560;
    spectCapXSects[942440] = 1.065;
    spectCapXSects[952410] = 64.41;
    spectCapXSects[952420] = 45.84; //meta-state of Americium-242
    spectCapXSects[952430] = 42.10;
    spectCapXSects[962420] = 5.341;
    spectCapXSects[962430] = 7.181;
    spectCapXSects[962440] = 13.84;
    spectCapXSects[962450] = 14.22;
    spectCapXSects[962460] = 2.987;
    spectCapXSects[962470] = 12.02;
    spectCapXSects[962480] = 6.516;
    spectCapXSects[962500] = 0.08555;
    spectCapXSects[982490] = 29.13;
    spectCapXSects[982500] = 293.4;
    spectCapXSects[982510] = 128.8;
    spectCapXSects[982520] = 2.139;

    break;

  case fast :
    spectCapXSects[922340] = 0.6114;
    spectCapXSects[922350] = 0.53;
    spectCapXSects[922360] = 0.5514;
    spectCapXSects[922380] = 0.28;
    spectCapXSects[932370] = 1.43;
    spectCapXSects[942380] = 0.69;
    spectCapXSects[942390] = 0.47;
    spectCapXSects[942400] = 0.48;
    spectCapXSects[942410] = 0.44;
    spectCapXSects[942420] = 0.4;
    spectCapXSects[942440] = 0.2323;
    spectCapXSects[952410] = 1.32;
    spectCapXSects[952420] = 0.36; //meta-state of Americium-242
    spectCapXSects[952430] = 1.01;
    spectCapXSects[962420] = 0.31;
    spectCapXSects[962430] = 0.23;
    spectCapXSects[962440] = 0.79;
    spectCapXSects[962450] = 0.3;
    spectCapXSects[962460] = 0.22;
    spectCapXSects[962470] = 0.3054;
    spectCapXSects[962480] = 0.2481;
    spectCapXSects[962500] = 0.001453;
    spectCapXSects[982490] = 0.3635;
    spectCapXSects[982500] = 0.4209;
    spectCapXSects[982510] = 0.3122;
    spectCapXSects[982520] = 0.2905;

    break;

  default :
    throw GenException("Error, requested average sigma_gamma for unsupported spectrum");
  }

  avgCapXSects.insert(make_pair(s, spectCapXSects));
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Material::initAvgFisXSects(Spectrum s)
{
  map<Iso, double> spectFisXSects;        

  switch(s) {

  case thermal :
    spectFisXSects[922340] = 0.4911;
    spectFisXSects[922350] = 22.69;
    spectFisXSects[922360] = 0.2166;
    spectFisXSects[922380] = 0.1105;
    spectFisXSects[932370] = 0.5713;
    spectFisXSects[942380] = 2.033;
    spectFisXSects[942390] = 46.45;
    spectFisXSects[942400] = 0.6222;
    spectFisXSects[942410] = 54.52;
    spectFisXSects[942420] = 0.5354;
    spectFisXSects[942440] = 0;
    spectFisXSects[952410] = 0.8959;
    spectFisXSects[952420] = 224.4; //meta-state of Americium-242
    spectFisXSects[952430] = 0.4455;
    spectFisXSects[962420] = 0.4676;
    spectFisXSects[962430] = 57.64;
    spectFisXSects[962440] = 0.9332;
    spectFisXSects[962450] = 85.93;
    spectFisXSects[962460] = 0.6343;
    spectFisXSects[962470] = 18.94;
    spectFisXSects[962480] = 0.8273;
    spectFisXSects[962500] = 0;
    spectFisXSects[982490] = 86.9;
    spectFisXSects[982500] = 1.002;
    spectFisXSects[982510] = 285.3;
    spectFisXSects[982520] = 5.183;

    break;

  case fast :
    spectFisXSects[922340] = 0.3707;
    spectFisXSects[922350] = 1.87;
    spectFisXSects[922360] = 0.1192;
    spectFisXSects[922380] = 0.05;
    spectFisXSects[932370] = 0.39;
    spectFisXSects[942380] = 1.19;
    spectFisXSects[942390] = 1.82;
    spectFisXSects[942400] = 0.42;
    spectFisXSects[942410] = 2.44;
    spectFisXSects[942420] = 0.31;
    spectFisXSects[942440] = 0.2488;
    spectFisXSects[952410] = 0.35;
    spectFisXSects[952420] = 3.9; //meta-state of Americium-242
    spectFisXSects[952430] = 0.27;
    spectFisXSects[962420] = 0.2;
    spectFisXSects[962430] = 2.6;
    spectFisXSects[962440] = 0.49;
    spectFisXSects[962450] = 2.59;
    spectFisXSects[962460] = 0.32;
    spectFisXSects[962470] = 1.934;
    spectFisXSects[962480] = 0.3433;
    spectFisXSects[962500] = 0;
    spectFisXSects[982490] = 2.645;
    spectFisXSects[982500] = 1.102;
    spectFisXSects[982510] = 2.309;
    spectFisXSects[982520] = 0.7618;

    break;

  default :
    throw GenException("Error, requested average sigma_fission for unsupported spectrum");
  }

  avgFisXSects.insert(make_pair(s, spectFisXSects));
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Material::initAvgNus(Spectrum s)
{
  map<Iso, double> spectNus;

  switch(s) {

  case thermal :
    spectNus[922340] = 2.631;
    spectNus[922350] = 2.421;
    spectNus[922360] = 2.734;
    spectNus[922380] = 2.801;
    spectNus[932370] = 3.005;
    spectNus[942380] = 2.833;
    spectNus[942390] = 2.875;
    spectNus[942400] = 3.135;
    spectNus[942410] = 2.934;
    spectNus[942420] = 3.28;
    spectNus[942440] = 3.3;
    spectNus[952410] = 3.277;
    spectNus[952420] = 3.162; //meta-state of Americium-242
    spectNus[952430] = 3.732;
    spectNus[962420] = 3.746;
    spectNus[962430] = 3.434;
    spectNus[962440] = 3.725;
    spectNus[962450] = 3.832;
    spectNus[962460] = 3.858;
    spectNus[962470] = 3.592;
    spectNus[962480] = 3.796;
    spectNus[962500] = 3.8;
    spectNus[982490] = 4.062;
    spectNus[982500] = 3.97;
    spectNus[982510] = 4.14;
    spectNus[982520] = 4.126;

    break;

  default :
    throw GenException("Error, requested average nus for unsupported spectrum");
  }

  avgNus.insert(make_pair(s, spectNus));
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const map<Iso, double>& Material::getAvgCapXSects(Spectrum s)
{
  if (avgCapXSects.find(s) == avgCapXSects.end())
    throw GenException("Error, requested average capture cross sections for unsupported spectrum.");

  return avgCapXSects[s];
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const map<Iso, double>& Material::getAvgFisXSects(Spectrum s)
{
  if (avgFisXSects.find(s) == avgFisXSects.end())
    throw GenException("Error, requested average fission cross sections for unsupported spectrum.");

  return avgFisXSects[s];
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const map<Iso, double>& Material::getAvgNus(Spectrum s)
{
  if (avgNus.find(s) == avgNus.end())
    throw GenException("Error, requested average nu values for unsupported spectrum.");

  return avgNus[s];
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double Material::computeNeutWeight(const map<Iso, NumDens> &comp, Spectrum s) 
{
	// KDHFLAG this whole neutronics weighting scheme probably still needs work. 

  // Found error in eta term before; starting over...

  // If we haven't populated the data yet, do it now.
  if (avgCapXSects.find(s) == avgCapXSects.end())
    initAvgCapXSects(s);
  if (avgFisXSects.find(s) == avgFisXSects.end())
    initAvgFisXSects(s);
  if (avgNus.find(s) == avgNus.end())
    initAvgNus(s);

  // Right now, neutron weighting values are just eta

  // eta = sum_i(nu_i * sigma_f_i * N_i) / sum_i(sigma_a_i * N_i)
  int i;
  double N_i;
  double numerator = 0;
  double denominator = 0;
  double nu_i;
  double sigma_f_i;
  double sigma_gamma_i;

  // Loop through the composition.
  map<Iso, NumDens>::const_iterator iterLoop;
  map<Iso, double>::const_iterator iterFind;
  for (iterLoop = comp.begin(); iterLoop != comp.end(); iterLoop ++) {
    i = iterLoop->first;
    N_i = iterLoop->second;
                
    // Get nu, cross-sections.
    iterFind = avgNus.find(s)->second.find(i);
    if (iterFind != avgNus.find(s)->second.end())
      nu_i = iterFind->second;
    else
      nu_i = 0;

    iterFind = avgFisXSects.find(s)->second.find(i);
    if (iterFind != avgFisXSects.find(s)->second.end())
      sigma_f_i = iterFind->second;
    else
      sigma_f_i = 0;

    iterFind = avgCapXSects.find(s)->second.find(i);
    if (iterFind != avgCapXSects.find(s)->second.end())
      sigma_gamma_i = iterFind->second;
    else
      sigma_gamma_i = 0;
                
    // If sigma_f_i != 0.0, this is a fuel isotope and we should count
    // its contribution.
    if (sigma_f_i != 0.0) {
      numerator += nu_i * sigma_f_i * N_i;
      denominator += (sigma_f_i + sigma_gamma_i) * N_i;
    }
  }

  if (0 == denominator) 
    return 0;
  else    
    return numerator / denominator;// * targetNumDens);

  // // This function handles assigning a reactivity weighting based purely on the
  // // fission contribution for each isotope to the k_inf for a requested fuel
  // // recipe so that a close approximation can be made based upon the available
  // // separated fuel streams.  

  // // Assumes that the P_tnl and P_fnl are the same for the reactor the fuel
  // // would be used within.

  // // Assume that f = 1 for now.  Later need to calculate f for each
  // // individual fuel recipe, if not for each reactor based on moderator and
  // // such.

  // // Assume that the fast fission factors, p and epsilon, sum to 1.
  // // Not a really bad assumption for fast reactors, but does contribute to
  // // discrepancies when doing thermal reactor comparisons.
        
  // double eta_i;
  // double f_i;
  // double weight_i;
  // double k_inf_i;
  // map<Iso, NumDens>::iterator iter; // composition iterator
  // map<Iso, NumDens>::iterator numIter; // number density fraction iterator;
  // map<Iso, NumDens>::iterator weightIter; // weight iterator
  // double numDens_i;
  // //   double numDensTot = 0;
  // double weightComp = 0; // RWV for the composition of interest

  // map<Iso, double> weightInd; // map for holding indep rwv for each isotope
  // map<Iso, double> weightDep; // num density based rwv for each isotope
  // map<Iso, double> massFrac;

  // // If we haven't populated the data yet, do it now.
  // if (avgCapXSects.find(s) == avgCapXSects.end())
  //      initAvgCapXSects(s);
  // if (avgFisXSects.find(s) == avgFisXSects.end())
  //      initAvgFisXSects(s);
  // if (avgNus.find(s) == avgNus.end())
  //      initAvgNus(s);

  // // const map<Iso, double>& csFis = Material::getAvgFisXSects();
  // // const map<Iso, double>& csCap = Material::getAvgCapXSects();
  // // const map<Iso, double>& avNu = Material::getAvgNus();

  // // MAKE THE NUMBER DENSITY SUMMATION
  //  // for(numIter = comp.begin(); numIter != comp.end(); numIter ++)
  //  // {
  //  //   Iso j = (*numIter).first;
  //  //   numDensTot = comp[j]+numDensTot;
  //  // }

  //  // ** Need to populate for each isotope with map data
  //  for(iter = candComp.begin(); iter != candComp.end(); iter ++)
  //  {
  //       Iso i = (*iter).first;

  //       // Roy--FYI, these calls aren't really safe, because we might not
  //       // have the isotope in question in our data. Need to manually
  //       // make sure that csFis.find(i) != csFis.end() before accessing
  //       // the data (and use zero or whatever if the isotope isn't
  //       // found); otherwise we get random garbage. I didn't notice this
  //       // myself until I started running the matching test problems,
  //       // which include non-fissionable that we haven't entered data
  //       // for. ~KMO

  //       // double fis_i = csFis.find(i)->second;
  //       // double cap_i = csCap.find(i)->second;
  //       // double nu_i = avNu.find(i)->second;


  //       eta_i = Material::getEta(i);//nu_i*(fis_i/(fis_i+cap_i));
  //       f_i = 1; // (fis_i+cap_i)/(fis_i+cap_i); // f = 1
  //       k_inf_i = eta_i*f_i;
  //       weight_i = k_inf_i-1; // RWV for k_eff = 1

  //       // MAKE THE NUMBER DENSITY FOR ISOTOPE
  //       numDens_i = candComp[i];

  //       // Just using eta for now. ~KMO
  //       weightInd[i] = eta_i;//weight_i; // creates indep rwv 2nd map parameter

  //       // MAKE RELATIVE RWV FOR ISOTOPE
  //       weightDep[i] = numDens_i*weightInd[i];
  //  }
        
  // // ** Need to use map data for each isotope to calculate total reactivity.
  // // RhoComp should = 0 for critical fuel in thermal reactor.
  // // Build a warning and error message to ensure rhoComp = ~0.

  //  for(weightIter = weightDep.begin(); weightIter != weightDep.end(); weightIter ++)
  //  {
  //       Iso k = (*weightIter).first;
  //       weightComp = weightDep[k]+weightComp;
  //  }

  //  // Calculate target number density.
  //  double numDensTarget = 0;
  //  for (numIter = targetComp.begin(); numIter != targetComp.end(); numIter ++)
  //       numDensTarget += numIter->second;

  //  return weightComp / numDensTarget; // numDensTot;

}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double Material::computeNeutWeight(Spectrum s) const
{
  return Material::computeNeutWeight(this->getComp(), s);
}
