// Material.cpp
#include "Material.h"

#include "CycException.h"
#include "Timer.h"
#include "Logger.h"

#include <cmath>
#include <vector>

using namespace std;

std::vector<mat_rsrc_ptr> Material::materials_;

bool Material::decay_wanted_ = false;

int Material::decay_interval_ = 1;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
Material::Material() {
  last_update_time_ = TI->time();
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
Material::Material(IsoVector comp) {
  last_update_time_ = TI->time();
  iso_vector_ = comp;
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
Material::Material(const Material& other) {
  iso_vector_ = other.iso_vector_;
  last_update_time_ = other.last_update_time_;
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Material::absorb(mat_rsrc_ptr matToAdd) {
  // Get the given Material's composition.
  IsoVector vec_to_add = matToAdd->isoVector();
  iso_vector_ = iso_vector_ + vec_to_add;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
mat_rsrc_ptr Material::extract(double mass) {
  IsoVector new_comp = iso_vector_;
  new_comp.setMass(mass);

  iso_vector_ = iso_vector_ - new_comp;
  
  return mat_rsrc_ptr(new Material(new_comp));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
mat_rsrc_ptr Material::extract(IsoVector rem_comp) {
  iso_vector_ = iso_vector_ - rem_comp;
  return mat_rsrc_ptr(new Material(rem_comp));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void Material::print() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
rsrc_ptr Material::clone() {
  rsrc_ptr mat(new Material(*this));
  return mat;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
bool Material::checkQuality(rsrc_ptr other){
  // This will be false until proven true
  bool toRet = false;
  IsoVector lhs_vec = iso_vector_;

  try {
    // Make sure the other is a material
    IsoVector rhs_vec = boost::dynamic_pointer_cast<Material>(other)->isoVector();
    toRet = (lhs_vec==rhs_vec);
  } catch (std::exception& e) {
    toRet = false;
  }

  return toRet;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
bool Material::checkQuantityEqual(rsrc_ptr other) {
  // This will be false until proven true
  bool toRet = false;

  // Make sure the other is a material
  try{
    // check mass values
    double second_qty = boost::dynamic_pointer_cast<Material>(other)->quantity();
    toRet=( abs(quantity() - second_qty) < EPS_KG);
  } catch (std::exception e) {
  }
  return toRet;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
bool Material::checkQuantityGT(rsrc_ptr other){
  // true if the total atoms in the other is greater than in the base.
  // This will be true until proven false
  bool toRet = false;

  // Make sure the other is a material
  try{
    // check mass values
    double second_qty = boost::dynamic_pointer_cast<Material>(other)->quantity();
    toRet = second_qty - quantity() > EPS_KG;
  } catch (std::exception& e){
  }

  return toRet;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Material::decay() {
  int curr_time = TI->time();
  int delta_time = curr_time - last_update_time_;
  
  iso_vector_.executeDecay(delta_time);

  last_update_time_ = curr_time;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Material::decayMaterials(int time) {
  // if decay is on
  if (decay_wanted_) {
    // and if (time(mod interval)==0)
    if (time % decay_interval_ == 0) {
      // acquire a list of all materials
      for (vector<mat_rsrc_ptr>::iterator mat = materials_.begin();
          mat != materials_.end();
          mat++){
         // and decay each of them
         (*mat)->decay();
      }
    }
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Material::setDecay(int dec) {
  if ( dec <= 0 ) {
    decay_wanted_ = false;
  } else if ( dec > 0 ) {
    decay_wanted_ = true;
    decay_interval_ = dec;
  }
}
