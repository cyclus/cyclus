// Material.cpp
#include <iostream>

using namespace std;

#include "Material.h"

#include "Utility/MassTable.h"
#include "GenException.h"
#include "Logician.h"
#include "Timer.h"


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
Material::Material(): atomEqualsMass(true), total_mass(0), total_atoms(0) 
{
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
Material::Material(xmlNodePtr cur)
{
  
  recipeName = XMLinput->get_xpath_content(cur,"name");

  string comp_type = XMLinput->get_xpath_content(cur,"basis");
  CompMap &comp_map = ( "atom" != comp_type ? massHist[TI->getTime()] : 
                       compHist[TI->getTime()]);
  double &total_comp = ( "atom" != comp_type ? total_mass : total_atoms);

  units = XMLinput->get_xpath_content(cur,"unit");
  
  total_comp = atoi(XMLinput->get_xpath_content(cur,"total"));

  xmlNodeSetPtr isotopes = XMLinput->get_xpath_elements(cur,"isotope");

  for (int i=0;i<isotopes->nodeNr;i++)
  {
    xmlNodePtr iso_node = isotopes->nodeTab[i];
    Iso isotope = atoi(XMLinput->get_xpath_content(iso_node,"id"));
    comp_map[isotope] = total_comp*atof(XMLinput->get_xpath_content(iso_node,"comp"));
  }
  

  if ( "atom" != comp_type)
    rationalize_M2A();
  else
    rationalize_A2M();

  facHist = FacHistory() ;
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
Material::Material(CompMap comp, string mat_unit, string rec_name)
{
  units = mat_unit;
  recipeName = rec_name;
  
  compHist[TI->getTime()] = comp;
  rationalize_A2M();
  total_atoms=this->getTotAtoms();
  total_mass=this->getTotMass();


  facHist = FacHistory() ;

}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
Material::Material(CompMap comp, string mat_unit, string rec_name, Mass size)  
{

  if(size==0){
    CompMap::iterator iter = comp.begin();
    while(iter!=comp.end()) {
      comp.erase(iter->first);
      comp.insert(make_pair(iter->first, 0));
      iter++;
    }
  }

  compHist[TI->getTime()] = comp;
  units = mat_unit;
  recipeName = rec_name;

  facHist = FacHistory() ;

  total_mass=this->getTotMass();
  total_atoms=this->getTotAtoms();

  rationalize_A2M();

  Mass diff = (this->getTotMass() - size);

  if(diff == 0 && size>0){
    // do nothing, you've already created a material of the right magnitude
  }
  else if(diff > 0 && size>0 ){
    this->extractMass(diff);
  }
  else if(diff < 0 && size>0){
    this->addMass(-diff);
  }


}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void Material::load_recipes()
{

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
void Material::load_recipebook(string filename, string ns, string format)
{
  XMLinput->extendCurNS(ns);

  if ("xml" == format)
    XMLinput->load_recipebook(filename);
  else
    throw GenException(format + "is not a supported recipebook format.");

  XMLinput->stripCurNS();
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const bool Material::isNeg(Iso tope) const
{
  if (this->getComp(tope) == 0)
    return false;

  Atoms atoms_eps =  eps *1e3 / Material::getMassNum(tope); 
  return (this->getComp(tope) + atoms_eps < 0);
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const bool Material::isZero(Iso tope) const
{
  Atoms atoms_eps =  AVOGADRO / Material::getMassNum(tope) * eps * 1e6; 
  return fabs(this->getComp(tope)) < atoms_eps;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const double Material::getIsoMass(Iso tope) const
{
  map<Iso, Atoms> currComp = this->getComp();
  return Material::getIsoMass(tope, currComp);
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double Material::getIsoMass(Iso tope, const CompMap& comp)
{
  // If the given isotope is present, calculate and return its mass. 
  // Else return 0.

  CompMap::const_iterator searchIso = comp.find(tope);
  double mass = 0;

  if (searchIso != comp.end()) 
    mass = (*searchIso).second*Material::getMassNum(tope)/1e3;
  return mass;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const double Material::getEltMass(int elt) const
{
  map<Iso, Atoms> currComp = this->getComp();
  return Material::getEltMass(elt, currComp);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double Material::getEltMass(int elt, const map<Iso, Atoms>& comp)
{
  // Iterate through the current composition...
  map<Iso, Atoms>::const_iterator iter = comp.begin();
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
const Mass Material::getTotMass() const
{
  CompMap comp = this->getComp();
  return Material::getTotMass(comp);
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double Material::getTotMass(const CompMap& comp)
{
  // Sum the masses of the isotopes.
  CompMap::const_iterator iter = comp.begin();
  double mass = 0;

  while (iter != comp.end()) {
    mass = mass + Material::getIsoMass(iter->first, comp);
    iter ++;
  }
  return mass;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const Atoms Material::getTotAtoms() const
{
  CompMap comp = this->getComp();
  return Material::getTotAtoms(comp);
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double Material::getTotAtoms(const CompMap& comp)
{
  // Sum the atoms of the isotopes.
  CompMap::const_iterator iter = comp.begin();
  double atoms = 0;

  while (iter != comp.end()) {
    atoms = atoms + Material::getComp(iter->first, comp);
    iter ++;
  }
  return atoms;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Material::changeComp(Iso tope, Atoms change, int time)
{

  // If the composition has already changed during this timestep, we copy and 
  // delete the current entry, modify it, and re-add. If there's no entry 
  // for this timestep, simply copy the most recent one, edit, and add.
  CompMap newComp;
  if (compHist.end() != compHist.find(time)) {
    newComp = CompMap(compHist[time]);
    compHist.erase(time);
  }
  else
    newComp = CompMap((*compHist.rbegin()).second);

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
  // set the nd to zero.
  if (this->isZero(tope)) {
    CompMap newComp = compHist[time];
    newComp.erase(tope);
    newComp.insert(make_pair(tope, 0));
    compHist.insert(make_pair(time, newComp));
  }

  // If the value is negative, throw an exception;
  // something's gone wrong.
  if (this->isNeg(tope))
    throw GenException("Tried to make isotope composition negative.");

  rationalize_A2M();

}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const CompMap Material::getMassComp() const
{
  CompMap comp;
  MassHistory::const_reverse_iterator it = massHist.rbegin();
  if (it!=massHist.rend()){
    comp = it->second;
  }
  else{
    comp.insert(make_pair(Iso(92235),Atoms(0)));
  }
  return comp;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const Mass Material::getMassComp(Iso tope) const
{
  CompMap currComp = this->getMassComp();

  // If the isotope isn't currently present, return 0. Else return the 
  // isotope's current number density.
  if (currComp.find(tope) == currComp.end()) {
    return 0;
  }
  else
    return currComp[tope];
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double Material::getMassComp(Iso tope, const CompMap& comp)
{
  // If the given isotope is present, calculate and return its comp. 
  // Else return 0.

  CompMap::const_iterator searchIso = comp.find(tope);
  double mass = 0;
  if (searchIso != comp.end()) 
    // comp = searchIso->second * Material::getMassNum(tope)/ AVOGADRO / 1e6;
    mass = (*searchIso).second;
  return mass;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const CompMap Material::getComp() const
{
  CompMap comp;
  CompHistory::const_reverse_iterator it = compHist.rbegin();
  if (it!=compHist.rend()){
    comp = it->second;
  }
  else{
    comp.insert(make_pair(Iso(92235),Atoms(0)));
  }
  return comp;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double Material::getComp(Iso tope, const CompMap& comp)
{
  // If the given isotope is present, calculate and return its comp. 
  // Else return 0.

  CompMap::const_iterator searchIso = comp.find(tope);
  double atoms = 0;
  if (searchIso != comp.end()) 
    // comp = searchIso->second * Material::getMassNum(tope)/ AVOGADRO / 1e6;
    atoms = (*searchIso).second;
  return atoms;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const CompMap Material::getFracComp(double frac) const
{
  // Create a new composition object.
  CompMap newComp;

  // Iterate through the current composition vector and add to the new object 
  // the specified fraction of each isotope.
  CompMap currComp = this->getComp();
  CompMap::iterator iter = currComp.begin();

  while (iter != currComp.end()) {
    newComp[iter->first] = iter->second * frac;
    iter ++;
  }

  return newComp;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const Atoms Material::getComp(Iso tope) const
{
  CompMap currComp = this->getComp();

  // If the isotope isn't currently present, return 0. Else return the 
  // isotope's current number density.
  if (currComp.find(tope) == currComp.end()) {
    return 0;
  }
  else
    return currComp[tope];
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Material::absorb(Material* matToAdd)
{
  // Get the given Material's composition.
  CompMap compToAdd = matToAdd->getComp();

  // Iterate over the isotopes in the Material we're adding and add them to 
  // this Material.
  CompMap::const_iterator iter = compToAdd.begin();
  Iso isoToAdd;
  Atoms atomsToAdd;

  while (*iter != *(compToAdd.end())) {
    isoToAdd = iter->first;
    atomsToAdd = iter->second;
    this->changeComp(isoToAdd, atomsToAdd, TI->getTime());
    iter ++;
  }

  // Delete the given Material.
  delete matToAdd;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Material::extract(Material* matToRem)
{
  // Get the given Material's composition.
  CompMap compToRem = matToRem->getComp();

  // Iterate over the isotopes in the Material we're removing and subtract 
  // them from this Material.
  CompMap::iterator iter = compToRem.begin();
  Iso isoToRem;
  Atoms ndToRem;

  while (*iter != *(compToRem.end())) {
    isoToRem = iter->first;
    ndToRem = 0 - iter->second;
    this->changeComp(isoToRem, ndToRem, TI->getTime());
    iter ++;
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int Material::getAtomicNum(Iso tope)
{
  // Make sure the number's in a reasonable range.
  if (tope < 1010 || tope > 1182949)
    throw GenException("Tried to get atomic number of invalid isotope");

  // Get the atomic number and return.
  return tope / 1000; // integer division
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int Material::getMassNum(Iso tope)
{
  // Make sure the number's in a reasonable range.
  if (tope < 1010 || tope > 1182949)
    throw GenException("Tried to get atomic number of invalid isotope");

  // Get the mass number and return.
  // % 1000 means "modulo 1000"
  return (tope) % 1000;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Material* Material::extractMass(double mass)
{
  double frac = mass / this->getTotMass();
  CompMap comp = this->getFracComp(frac);
  Material* newMat = new Material(comp , units, " ");
  this->extract(newMat);
  return newMat;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Material* Material::addMass(double mass)
{
  double frac = mass / this->getTotMass();
  CompMap comp = this->getFracComp(frac);
  Material* newMat = new Material(comp , units, " ");
  this->absorb(newMat);
  return newMat;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void Material::rationalize_A2M()
{
  total_atoms = this->getTotAtoms();

  // loop through each isotope in the composition for the current time.
  for(CompMap::iterator entry = compHist[TI->getTime()].begin();
      entry != compHist[TI->getTime()].end();
      entry++)
  {
    // multiply the number of atoms by the mass number of that isotope and convert to kg
    massHist[TI->getTime()][(*entry).first] = (*entry).second*getMassNum((double)(*entry).first)/1e3;
  }
  total_mass = this->getTotMass();
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void Material::rationalize_M2A()
{

  total_mass = this->getTotMass();

  for(CompMap::iterator entry = massHist[TI->getTime()].begin();
      entry != massHist[TI->getTime()].end();
      entry++)
  {
    compHist[TI->getTime()][(*entry).first] = (*entry).second*1e3 / getMassNum((*entry).first);
  }

  total_atoms = this->getTotAtoms();
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void Material::print(){
    printComp("Atom composition:", compHist[TI->getTime()]);
    cout << "\tTotal atoms: " << this->getTotAtoms() 
        << " moles per " << units << endl;
    printComp("Mass composition:", massHist[TI->getTime()]);
    cout << "\tTotal mass: " << this->getTotMass() 
        << " kg per " << units << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void Material::printComp(string header, CompMap comp_map)
{

  cout << "\t" << header << endl;
  for (CompMap::iterator iso = comp_map.begin();
       iso != comp_map.end();
       iso++)
  cout << "\t" << (*iso).first << " : " <<  (*iso).second << endl;
  
}


