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
Material::Material(IsoVector comp) {
  last_update_time_ = TI->time();
  iso_vector_ = comp;
  CLOG(LEV_INFO4) << "Material ID=" << ID_ << " was created.";
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
Material::Material(const Material& other) {
  iso_vector_ = other.iso_vector_;
  last_update_time_ = other.last_update_time_;
  CLOG(LEV_INFO4) << "Material ID=" << ID_ << " was created.";
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Material::absorb(mat_rsrc_ptr matToAdd) { 
  // @gidden figure out how to handle this with the database - mjg
  // Get the given Material's composition.
  IsoVector vec_to_add = matToAdd->isoVector();
  iso_vector_ = iso_vector_ + vec_to_add;
  CLOG(LEV_DEBUG2) << "Material ID=" << ID_ << " absorbed material ID="
                   << matToAdd->ID() << ".";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
mat_rsrc_ptr Material::extract(double mass) {

  IsoVector new_comp = iso_vector_;
  new_comp.setMass(mass);
  iso_vector_ = iso_vector_ - new_comp;

  CLOG(LEV_DEBUG2) << "Material ID=" << ID_ << " had " << mass
                   << " kg extracted from it. New mass=" << quantity() << " kg.";
  
  mat_rsrc_ptr new_mat = new Material(new_comp);
  // we just split a resource, so keep track of the original for book keeping
  new_mat->setOriginalID( this->originalID() );

  return mat_rsrc_ptr(new_mat);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
mat_rsrc_ptr Material::extract(IsoVector rem_comp) {
  iso_vector_ = iso_vector_ - rem_comp;

  CLOG(LEV_DEBUG2) << "Material ID=" << ID_ << " had vector extracted.";

  mat_rsrc_ptr new_mat = new Material(rem_comp);
  // we just split a resource, so keep track of the original for book keeping
  new_mat->setOriginalID( this->originalID() );

  return mat_rsrc_ptr(new_mat);
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
  vector<string> entries = iso_vector_.compStrings();
  for (entry = entries.begin(); entry != entries.end(); entry++) {
    CLOG(LEV_INFO5) << "   " << *entry;
  }
  return "";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
void Material::setQuantity(double quantity) {
  iso_vector_.setMass(quantity);
  CLOG(LEV_DEBUG2) << "Material ID=" << ID_ << " had mass set to"
                   << quantity << " kg";
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -    
rsrc_ptr Material::clone() {
  CLOG(LEV_DEBUG2) << "Material ID=" << ID_ << " was cloned.";
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
void Material::decay() {
  int curr_time = TI->time();
  int delta_time = curr_time - last_update_time_;
  
  iso_vector_.executeDecay(delta_time);

  last_update_time_ = curr_time;
  // we've decayed something, so register the state change
  this->addToTable();
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
void Material::define_table() {
  // declare the table columns
  column id("ID","INTEGER");
  column state_id("StateID","INTEGER");
  column time("Time","INTEGER");
  // declare the table's primary key
  primary_key pk;
  pk.push_back("ID"), pk.push_back("StateID");
  material_table->setPrimaryKey(pk);
  // add columns to the table
  material_table->addColumn(id);
  material_table->addColumn(state_id);
  material_table->addColumn(time);
  // add foreign keys
  foreign_key_ref *fkref;
  foreign_key *fk;
  key myk, theirk;
  //    Resources table foreign keys
  theirk.push_back("ID");
  fkref = new foreign_key_ref("Resources",theirk);
  //      the resource id
  myk.push_back("ID");
  fk = new foreign_key(myk, (*fkref) );
  material_table->addForeignKey( (*fk) ); // id references resources' id
  myk.clear(), theirk.clear();
  //    IsotopicStates table foreign keys
  theirk.push_back("ID");
  fkref = new foreign_key_ref("IsotopicStates",theirk);
  //      the state id
  myk.push_back("StateID");
  fk = new foreign_key(myk, (*fkref) );
  material_table->addForeignKey( (*fk) ); // stateid references isotopicstates' id
  myk.clear(), theirk.clear();
  // we've now defined the table
  material_table->tableDefined();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Material::addToTable(){
  // if we haven't logged an material yet, define the table
  if ( !material_table->defined() ) {
    Material::define_table();
  }

  iso_vector_.recordState();

  // make a row
  // declare data
  data an_id( this->originalID() ), a_state( this->stateID() ), // @MJG FLAG need to do state recording
    a_time( TI->time() );
  // declare entries
  entry id("ID",an_id), state("StateID",a_state), time("Time",a_time);
  // declare row
  row aRow;
  aRow.push_back(id), aRow.push_back(state), aRow.push_back(time);
  // add the row
  material_table->addRow(aRow);
  // record this primary key
  pkref_.push_back(id);
  pkref_.push_back(state);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Material::setOriginatorID(int id){
  originatorID_ = id;
  this->Resource::addToTable();
  this->Material::addToTable();
}
