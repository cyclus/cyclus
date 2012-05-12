// Material.cpp
#include "Material.h"

#include "CycException.h"
#include "Timer.h"
#include "Logger.h"

#include <cmath>
#include <vector>

using namespace std;

vector<mat_rsrc_ptr> Material::materials_;

bool Material::decay_wanted_ = false;

int Material::decay_interval_ = 1;

table_ptr Material::material_table = new Table("MaterialHistory"); 

bool Material::type_is_logged_ = false;


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
Material::Material() {
  last_update_time_ = TI->time();
  CLOG(LEV_INFO4) << "Material ID=" << ID_ << " was created.";
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
Material::Material(CompMapPtr comp) {
  IsoVector vec = IsoVector(comp);
  last_update_time_ = TI->time();
  iso_vector_ = vec;
  CLOG(LEV_INFO4) << "Material ID=" << ID_ << " was created.";
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
Material::Material(IsoVector vec) {
  last_update_time_ = TI->time();
  iso_vector_ = vec;
  CLOG(LEV_INFO4) << "Material ID=" << ID_ << " was created.";
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
Material::Material(const Material& other) {
  iso_vector_ = other.iso_vector_;
  last_update_time_ = other.last_update_time_;
  quantity_ = other.quantity_;
  CLOG(LEV_INFO4) << "Material ID=" << ID_ << " was created.";
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Material::absorb(mat_rsrc_ptr matToAdd) { 
  // @gidden figure out how to handle this with the database - mjg
  // Get the given Material's composition.
  double amt = matToAdd->quantity();
  iso_vector_.mix(matToAdd->isoVector(),quantity_/amt); // @MJG_FLAG this looks like it copies isoVector()... should this return a pointer?
  quantity_ += amt;
  CLOG(LEV_DEBUG2) << "Material ID=" << ID_ << " absorbed material ID="
                   << matToAdd->ID() << ".";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
mat_rsrc_ptr Material::extract(double mass) {
  // remove our mass
  quantity_ -= mass;
  // make a new material, set its mass
  mat_rsrc_ptr new_mat = mat_rsrc_ptr(new Material(iso_vector_));
  new_mat->setQuantity(mass);
  // we just split a resource, so keep track of the original for book keeping
  new_mat->setOriginalID( this->originalID() );

  CLOG(LEV_DEBUG2) << "Material ID=" << ID_ << " had " << mass
                   << " kg extracted from it. New mass=" << quantity() << " kg.";
  
  return new_mat;
}

// //- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// mat_rsrc_ptr Material::extract(IsoVector rem_comp) {
//   iso_vector_ = iso_vector_ - rem_comp;

//   CLOG(LEV_DEBUG2) << "Material ID=" << ID_ << " had vector extracted.";

//   mat_rsrc_ptr new_mat = new Material(rem_comp);
//   // we just split a resource, so keep track of the original for book keeping
//   new_mat->setOriginalID( this->originalID() );

//   return mat_rsrc_ptr(new_mat);
// }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void Material::print() {
  CLOG(LEV_INFO4) << "Material ID=" << ID_
                   << ", quantity=" << quantity() << ", units=" << units();

  CLOG(LEV_INFO5) << "Composition {";
  CLOG(LEV_INFO5) << detail();
  CLOG(LEV_INFO5) << "}";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
string Material::detail() {
  vector<string>::iterator entry;
  vector<string> entries = iso_vector_.comp()->compStrings();
  for (entry = entries.begin(); entry != entries.end(); entry++) {
    CLOG(LEV_INFO5) << "   " << *entry;
  }
  return "";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void Material::setQuantity(double quantity) {
  quantity_ = quantity;
  CLOG(LEV_DEBUG2) << "Material ID=" << ID_ << " had mass set to"
                   << quantity << " kg";
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
double Material::quantity() {
  return quantity_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
rsrc_ptr Material::clone() {
  CLOG(LEV_DEBUG2) << "Material ID=" << ID_ << " was cloned.";
  rsrc_ptr mat(new Material(*this));
  mat->setQuantity(this->quantity());
  return mat;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
bool Material::checkQuality(rsrc_ptr other){
  // This will be false until proven true
  bool toRet = false;
  IsoVector lhs_vec = iso_vector_;

  try {
    // Make sure the other is a material
    mat_rsrc_ptr mat = boost::dynamic_pointer_cast<Material>(other);
    if (mat) {
      toRet = true;
    }
  } catch (...) { }

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
  } catch (exception e) { }
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
  } catch (exception& e){ }

  return toRet;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double Material::mass(Iso tope) {
  return iso_vector_.massFraction(tope) * quantity_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Material::decay() {
  int curr_time = TI->time();
  int delta_time = curr_time - last_update_time_;
  
  iso_vector_.decay((double)delta_time);

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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Material::addToTable() {
  Resource::addToTable();
  iso_vector_.log();
}
