// Material.cpp
#include "Material.h"

#include "CycArithmetic.h"
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
  setQuantity(0);
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
  setQuantity(0);
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
  double ratio = ((quantity_ < cyclus::eps_rsrc()) ? 1 : quantity_/amt);
  iso_vector_.mix(matToAdd->isoVector(),ratio); // @MJG_FLAG this looks like it copies isoVector()... should this return a pointer?
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
map<Iso, double> Material::diff(mat_rsrc_ptr other){
  CompMapPtr comp = CompMapPtr(other->isoVector().comp());
  double amt = other->mass(KG);
  return diff(comp, amt, KG);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
map<Iso, double> Material::diff(CompMapPtr other, double other_amt, MassUnit 
    unit){

  map<Iso, double>::iterator entry;
  CompMapPtr orig = CompMapPtr(this->unnormalizeComp(MASS, unit));
  map<Iso, double> to_ret = orig->map();
  double amt = 0;
  double orig_amt = this->mass(unit);
  for( entry= (*other).begin(); entry!= (*other).end(); ++entry ) {
    int iso = (*entry).first;
    if( orig->count(iso) != 0 ){
      amt = (*orig)[iso] - (*entry).second*other_amt ;
    } else {
      amt = -(*entry).second*other_amt;
    }
    to_ret[iso] = amt;
  }

  return to_ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
map<Iso, double> Material::applyThreshold(map<Iso, double> vec, double threshold){
  if(threshold < 0){
      stringstream ss;
      ss << "The threshold cannot be negative. The value provided was " 
         << threshold
         << " .";
      throw CycNegativeValueException(ss.str());
  }
  map<Iso, double>::iterator it;
  map<Iso, double> to_ret;

  for(it=vec.begin(); it!= vec.end(); ++it){
    if(abs((*it).second) > threshold) {
      to_ret[(*it).first] = (*it).second;
    }
  }
  return to_ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
mat_rsrc_ptr Material::extract(const CompMapPtr remove_comp, double remove_amt, 
    MassUnit unit, double threshold){
  CompMapPtr final_comp = CompMapPtr(new CompMap(MASS));
  vector<double> final_amt_vec = vector<double>();
  double final_amt = 0;

  map<Iso, double> remainder;
  remainder = applyThreshold(diff(remove_comp, remove_amt, unit), threshold);
  map<Iso, double>::iterator it;
  
  for(it=remainder.begin(); it!=remainder.end(); ++it){
    int iso = (*it).first;
    double amt = (*it).second;
    if(amt < 0){
      stringstream ss;
      ss << "The Material " << this->ID() 
         << " has insufficient material to extract "
         << " the isotope : " << iso 
         << ". The difference between the amounts is : "
         << amt ;
      throw CycNegativeValueException(ss.str());
    } else { 
      (*final_comp)[iso] = amt;
      final_amt_vec.push_back(amt);
    }
  }
  if(!final_amt_vec.empty()){
    final_amt = CycArithmetic::KahanSum(final_amt_vec);
  }

  // make new material
  mat_rsrc_ptr new_mat = mat_rsrc_ptr(new Material(remove_comp));
  new_mat->setQuantity(remove_amt, unit);
  new_mat->setOriginalID( this->originalID() ); // book keeping
  // adjust old material
  final_comp->normalize();
  this->iso_vector_ = IsoVector(CompMapPtr(final_comp)); 
  this->setQuantity(final_amt, unit);

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
bool Material::operator==(const mat_rsrc_ptr other){
  return almostEqual(other, 0);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
bool Material::almostEqual(const mat_rsrc_ptr other, double threshold) const {
  // I learned at 
  // http://www.ualberta.ca/~kbeach/comp_phys/fp_err.html#testing-for-equality
  // that the following is less naive than the naive way to do it... 
  return iso_vector_.comp()->almostEqual((other->isoVector().comp()), threshold);
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
CompMapPtr Material::unnormalizeComp(Basis basis, MassUnit unit){
  CompMapPtr norm_comp = isoVector().comp();
  double scaling;

  switch(basis) {
    case MASS :
      norm_comp->massify();
      scaling = this->mass(unit);
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
