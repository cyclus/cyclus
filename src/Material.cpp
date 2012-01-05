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
int Material::nextID_ = 0;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
Material::Material() { };

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
Material::Material(IsoVector comp) {
  comp_ = comp;
  //BI->registerMatChange(this);
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Material::absorb(Material* matToAdd) {
  // Get the given Material's composition.
  comp_ = comp_ + matToAdd->comp();

  delete matToAdd;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Material* Material::extract(double mass) {
  IsoVector new_comp = IsoVector(&comp_);
  new_comp.setMass(mass);

  comp_ = comp_ - new_comp;
  
  return new Material(new_comp);
  //BI->registerMatChange(this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Material* Material::extract(Material* matToRem) {
  IsoVector rem_comp = matToRem->comp();
  comp_ = comp_ - rem_comp;
  return new Material(rem_comp);
  //BI->registerMatChange(this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void Material::print() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
Material* clone() {
  return new Material(this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
bool Material::checkQuality(Resource* other){
  // This will be true until proven false
  bool toRet = true;

  // Make sure the other is a material
  try{
    CompMap second_comp_map = dynamic_cast<Material*>(other)->getAtomComp();
    // We need to check the recipe, isotope by isotope
    bool found_mismatch = false;
    CompMap::const_iterator iso = this->getAtomComp().begin();
    while (iso != this->getAtomComp().end() && !found_mismatch){
      if( second_comp_map.find((*iso).first)->second != (*iso).second){
        found_mismatch=true;
        toRet = false;
      }
      iso++;
    }
  } catch (Exception& e) {
    toRet = false;
  }

  return toRet;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
bool Material::checkQuantityEqual(Resource* other){
  // Here, we check that second.total_atoms_ = first.total_atoms
  // This will be true until proven false
  bool toRet = true;

  // Make sure the other is a material
  try{
    // check total_atoms values
    Atoms second_tot_atoms = dynamic_cast<Material*>(other)->getTotAtoms();
    // check total_mass values
    Mass second_tot_mass = dynamic_cast<Material*>(other)->getTotMass();
    toRet = ( total_mass_ - second_tot_mass < EPS_KG && total_atoms_ == second_tot_atoms);
  } catch (Exception& e) {
    toRet = false;
  }

  return toRet;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
bool Material::checkQuantityGT(Resource* other){
  // true if the total atoms in the other is greater than in the base.
  // Here, we check whether second.total_atoms_ > first.total_atoms
  // This will be true until proven false
  bool toRet = true;

  // Make sure the other is a material
  try{
    // check total_atoms values
    Atoms second_tot_atoms = dynamic_cast<Material*>(other)->getTotAtoms();
    // check total_mass values
    Mass second_tot_mass = dynamic_cast<Material*>(other)->getTotMass();
    toRet = ( second_tot_mass - total_mass_ < EPS_KG && second_tot_atoms > total_atoms_);
  } catch (Exception& e){
    toRet = false;
  }

  return toRet;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Material::decay() {
}

