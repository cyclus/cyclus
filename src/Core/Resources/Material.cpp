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

bool Material::type_is_recorded_ = false;


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
  if(quantity_ < mass){
    string err = "The mass ";
    err += mass;
    err += " cannot be extracted from Material with ID ";
    err += ID_; 
    throw CycNegativeValueException(err);
  }
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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
mat_rsrc_ptr Material::extract(const CompMapPtr other) {
 
  CompMapPtr new_comp = CompMapPtr(this->unnormalizeComp(MASS));
  assert(!new_comp->normalized());
  CompMapPtr remove_comp = other;
  double remainder_kg, new_kg;
  remainder_kg = this->quantity();
  for (CompMap::iterator it = remove_comp->begin(); 
       it != remove_comp->end(); it++) {
    // reduce isotope, if it exists in new_comp
    if ( this->mass(it->first) >= it->second ) {
      (*new_comp)[it->first] = this->mass(it->first) - it->second;
      new_kg += it->second;
      remainder_kg -= it->second;
    } else {
    stringstream ss("");
    ss << "The Material " << this->ID() 
      << " has insufficient material to extract the isotope : "
      << it->first ;
    throw CycNegativeValueException(ss.str());
    }
  }

  // make new material
  mat_rsrc_ptr new_mat = mat_rsrc_ptr(new Material(other));
  new_mat->setQuantity(new_kg, KG);
  new_mat->setOriginalID( this->originalID() ); // book keeping
  
  // adjust old material
  this->iso_vector_ = IsoVector(new_comp); 
  this->setQuantity(remainder_kg, KG);

  CLOG(LEV_DEBUG2) << "Material ID=" << ID_ << " had composition extracted.";

  return new_mat;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
mat_rsrc_ptr Material::extract(const mat_rsrc_ptr other) {
 
  CompMapPtr new_comp = CompMapPtr(unnormalizeComp(MASS));
  CompMapPtr remove_comp = other->isoVector().comp();

  int iso;
  double remainder_kg;
  remainder_kg = this->quantity();

  for (CompMap::iterator it = remove_comp->begin(); 
       it != remove_comp->end(); it++) {
    // reduce isotope, if it exists in new_comp
    iso = it->first;
    if ( this->mass(iso) >= other->mass(iso) ) {
      (*new_comp)[iso] = this->mass(iso) - other->mass(iso);
      remainder_kg -= other->mass(iso);
    } else {
    stringstream ss("");
    ss << "The Material " << this->ID() 
      << " has insufficient material to extract the isotope : "
      << iso ;
    throw CycNegativeValueException(ss.str());
    }
  }

  other->setOriginalID( this->originalID() ); // book keeping
  
  // adjust old material
  this->iso_vector_ = IsoVector(new_comp); 
  this->setQuantity(remainder_kg, KG);

  CLOG(LEV_DEBUG2) << "Material ID=" << ID_ << " had composition extracted.";

  return other;
}

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
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void Material::setQuantity(double quantity, MassUnit unit) {
  setQuantity( convertToKg(quantity,unit) );
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
double Material::quantity() {
  return quantity_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
double Material::mass(MassUnit unit) {
  return convertFromKg(quantity(),unit);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
double Material::mass(Iso tope, MassUnit unit) {
  return convertFromKg(mass(tope),unit);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
double Material::mass(Iso tope){
  double to_ret;
  CompMapPtr the_comp = isoVector().comp();
  the_comp->massify();
  if(the_comp->count(tope) != 0) {
    to_ret = the_comp->massFraction(tope)*mass(KG);
  } else {
    to_ret = 0;
  }
  return to_ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
double Material::convertFromKg(double mass, MassUnit to_unit) {
  double converted;
  switch( to_unit ) {
    case G :
      converted = mass*1000.0;
      break;
    case KG : 
      converted = mass;
      break;
    default:
      throw CycException("The unit provided is not a supported mass unit.");
  }
  return converted;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
double Material::convertToKg(double mass, MassUnit from_unit) {
  double in_kg;
  switch( from_unit ) {
    case G :
      in_kg = mass/1000.0;
      break;
    case KG : 
      in_kg = mass;
      break;
    default:
      throw CycException("The unit provided is not a supported mass unit.");
  }
  return in_kg;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
double Material::moles(){
  double m_a_ratio = isoVector().comp()->mass_to_atom_ratio();
  return mass(G)/m_a_ratio;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
double Material::moles(Iso tope){
  double to_ret;
  CompMapPtr the_comp = isoVector().comp();
  the_comp->atomify();
  if(the_comp->count(tope) != 0) {
    to_ret = moles()*isoVector().comp()->atomFraction(tope);
  } else {
    to_ret = 0;
  }
  return to_ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
CompMapPtr Material::unnormalizeComp(Basis basis){
  CompMapPtr norm_comp = isoVector().comp();
  double scaling;

  switch(basis) {
    case MASS :
      norm_comp->massify();
      scaling = this->mass(KG);
      break;
    case ATOM :
      norm_comp->atomify();
      scaling = this->moles();
      break;
    default : 
      throw CycException("The basis provided is not a supported CompMap basis");
  }
  CompMapPtr full_comp = CompMapPtr(new CompMap(*norm_comp));
  CompMap::iterator it;
  for( it=norm_comp->begin(); it!= norm_comp->end(); ++it ){
    (*full_comp)[it->first] = scaling*(it->second);
  }

  return full_comp;
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
  iso_vector_.record();
}
