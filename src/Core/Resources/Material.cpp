// Material.cpp
#include "Material.h"

#include "CycException.h"
#include "CycLimits.h"
#include "Timer.h"
#include "Logger.h"

#include <cmath>
#include <vector>
#include <list>

using namespace std;
using namespace boost;

list<Material*> Material::materials_;

bool Material::decay_wanted_ = false;

int Material::decay_interval_ = 1;

bool Material::type_is_recorded_ = false;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
Material::Material() {
  last_update_time_ = TI->time();
  CLOG(LEV_INFO4) << "Material ID=" << ID_ << " was created.";
  materials_.push_back(this);
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
Material::~Material() {
  materials_.remove(this);
}

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
mat_rsrc_ptr Material::extract(const CompMapPtr comp_to_rem, double kg_to_rem) {
 
  CompMapPtr new_comp = CompMapPtr(this->unnormalizeComp(MASS));
  CompMapPtr remove_comp = comp_to_rem;
  remove_comp->massify();
  assert(!new_comp->normalized());
  assert(remove_comp->normalized());

  double new_kg, kg_to_rem_i, remainder_kg, remainder_kg_i;
  remainder_kg = this->quantity();

  int iso;

  for (CompMap::iterator it = remove_comp->begin(); 
       it != remove_comp->end(); it++) {
    // get isotopic information
    kg_to_rem_i = it->second * kg_to_rem;
    if ( kg_to_rem_i <= cyclus::eps_rsrc() ) { kg_to_rem_i = 0; };
    iso = it->first;
    remainder_kg_i = this->mass(iso) - kg_to_rem_i;

    // check information
    if ( remainder_kg_i < -cyclus::eps_rsrc() ) {
      stringstream ss;
      ss << "The Material " << this->ID() 
         << " has insufficient material to extract the isotope : " << iso ;
      throw CycNegativeValueException(ss.str());
    } else if (remainder_kg_i <= cyclus::eps_rsrc()) {
      remainder_kg_i = 0; 
    }
    
    // operate on information
    (*new_comp)[iso] = remainder_kg_i;
    new_kg += kg_to_rem_i;
    remainder_kg -= kg_to_rem_i;
  }

  // make new material
  mat_rsrc_ptr new_mat = mat_rsrc_ptr(new Material(comp_to_rem));
  new_mat->setQuantity(new_kg, KG);
  new_mat->setOriginalID( this->originalID() ); // book keeping
  
  // adjust old material
  this->iso_vector_ = IsoVector(new_comp); 
  this->setQuantity(remainder_kg, KG);

  CLOG(LEV_DEBUG2) << "Material ID=" << ID_ << " had composition extracted.";

  return new_mat;
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
    mat_rsrc_ptr mat = dynamic_pointer_cast<Material>(other);
    if (mat) {
      toRet = true;
    }
  } catch (...) { }

  CLOG(LEV_DEBUG1) << "Material is checking quality, i.e. both are "
                   << "Materials, and the answer is " << toRet << " with true = " << true << ".";

  return toRet;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Material::decay() {
  int curr_time = TI->time();
  int delta_time = curr_time - last_update_time_;

  isoVector().decay(delta_time);
  last_update_time_ = curr_time;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Material::decayMaterials() {
  for (list<Material*>::iterator mat = materials_.begin();
      mat != materials_.end();
      mat++){
     (*mat)->decay();
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
bool Material::isMaterial(rsrc_ptr rsrc)
{
  mat_rsrc_ptr cast = dynamic_pointer_cast<Material>(rsrc);
  return !(cast.get() == 0);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Material::addToTable() {
  Resource::addToTable();
  iso_vector_.record();
}
