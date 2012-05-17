// Resource.cpp

#include "Resource.h"

// Resource IDs
int Resource::nextID_ = 0;

// Database table for resources
table_ptr Resource::resource_table = new Table("Resources"); 
table_ptr Resource::resource_type_table = new Table("ResourceTypes"); 

// -------------------------------------------------------------
Resource::Resource() {
  book_kept_ = false;
  ID_ = nextID_++;
  originalID_ = ID_;
  MLOG(LEV_DEBUG4) << "Resource ID=" << ID_ << ", ptr=" << this << " created.";
}

Resource::~Resource() {
  MLOG(LEV_DEBUG4) << "Resource ID=" << ID_ << ", ptr=" << this << " deleted.";
}

bool Resource::checkEquality(rsrc_ptr other) {
  bool toRet;
  (this->checkQuality(other) && this->checkQuantityEqual(other)) ? toRet = true : toRet = false;
  return toRet; 
}

void Resource::setOriginalID(int id){
  originalID_ = id;
}

// -------------------------------------------------------------
// -------- output database related members  -------- 

// Specific resources
void Resource::define_table(){
  // declare the table columns
  column id("ID","INTEGER");
  column type("Type","INTEGER");
  column amt("OriginalQuantity","REAL");
  // declare the table's primary key
  resource_table->setPrimaryKey(id);
  // add columns to the table
  resource_table->addColumn(id);
  resource_table->addColumn(type);
  resource_table->addColumn(amt);
  // add foreign keys
  foreign_key_ref *fkref;
  foreign_key *fk;
  key myk, theirk;
  //    Resource Types table foreign keys
  theirk.push_back("Type");
  fkref = new foreign_key_ref("ResourceTypes",theirk);
  //      the resource type
  myk.push_back("Type");
  fk = new foreign_key(myk, (*fkref) );
  resource_table->addForeignKey( (*fk) ); // type references Resource Types' type
  myk.clear(), theirk.clear();
  // we've now defined the table
  resource_table->tableDefined();
}

void Resource::addToTable(){
  if (book_kept_) {
    return;
  }
  book_kept_ = true;

  // if we haven't logged the resource type yet, do so
  if ( !this->is_resource_type_logged() ){
    Resource::logNewType();
    this->type_logged();
  }
  // if we haven't yet, define the table
  if ( !resource_table->defined() )
    Resource::define_table();
  
  // make a row
  // declare data
  data an_id( this->ID() ), a_type( (int)this->type() ), 
    an_amt( this->quantity() );
  // declare entries
  entry id("ID",an_id), type("Type",a_type), 
    amt("OriginalQuantity", an_amt);
  // declare row
  row aRow;
  aRow.push_back(id), aRow.push_back(type), 
    aRow.push_back(amt);
  // add the row
  resource_table->addRow(aRow);
  // record this primary key
  pkref_.push_back(id);
}

// Generic Resource Types
void Resource::define_type_table(){
  // declare the table columns
  column type("Type","INTEGER");
  column type_name("Name","VARCHAR(128)");
  column units("Units","VARCHAR(32)");
  // declare the table's primary key
  resource_type_table->setPrimaryKey(type);
  // add columns to the table
  resource_type_table->addColumn(type);
  resource_type_table->addColumn(type_name);
  resource_type_table->addColumn(units);
  // we've now defined the table
  resource_type_table->tableDefined();
}

void Resource::logNewType(){
  // if we haven't yet, define the table
  if ( !resource_type_table->defined() )
    Resource::define_type_table();
  
  // make a row
  // declare data
  data a_type( (int)this->type() ), a_name( this->type_name() ), a_unit( this->units() );
  // declare entries
  entry type("Type",a_type), name("Name", a_name), units("Units", a_unit);
  // declare row
  row aRow;
  aRow.push_back(type), aRow.push_back(name), aRow.push_back(units);
  // add the row
  resource_type_table->addRow(aRow);
  // record this primary key
  type_pkref_.push_back(type);
}
