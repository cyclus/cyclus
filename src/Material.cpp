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
  IsoVector new_comp = comp_;
  new_comp.setMass(mass);

  comp_ = comp_ - new_comp;
  
  return new Material(new_comp);
  //BI->registerMatChange(this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Material* Material::extract(IsoVector rem_comp) {
  comp_ = comp_ - rem_comp;
  return new Material(rem_comp);
  //BI->registerMatChange(this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void Material::print() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
Material* Material::clone() {
  return new Material(*this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
bool Material::checkQuality(Resource* other){
  // This will be true until proven false
  bool toRet = true;

  // Make sure the other is a material
  try{
    IsoVector second_comp = dynamic_cast<Material*>(other)->comp();
    // We need to check the recipe, isotope by isotope
    CompMap::const_iterator iso_iter = comp().comp().begin();
    while (iso_iter != comp().comp().end()){
      if( second_comp.atomCount(iso_iter->first) != iso_iter->second){
        toRet = false;
        break;
      }
      iso_iter++;
    }
  } catch (Exception& e) {
    toRet = false;
  }

  return toRet;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
bool Material::checkQuantityEqual(Resource* other){
  // This will be true until proven false
  bool toRet = false;

  // Make sure the other is a material
  try{
    // check mass values
    double second_qty = dynamic_cast<Material*>(other)->getQuantity();
    toRet = getQuantity() - second_qty < EPS_KG;
  } catch (Exception& e) {
  }

  return toRet;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
bool Material::checkQuantityGT(Resource* other){
  // true if the total atoms in the other is greater than in the base.
  // This will be true until proven false
  bool toRet = false;

  // Make sure the other is a material
  try{
    // check mass values
    double second_qty = dynamic_cast<Material*>(other)->getQuantity();
    toRet = second_qty - getQuantity() > EPS_KG;
  } catch (Exception& e){
  }

  return toRet;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Material::decay() {
}

