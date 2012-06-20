#include <iostream>

#include "Table.h"

#include "BookKeeper.h"
#include "Logger.h"
#include "CycException.h"

using namespace std;

// @gidden Indexing not currently supported
// @gidden Make generic creation and addition functions
// @gidden Add error checking for column names and data types
// @gidden move SQL-ness to Database

// -----------------------------------------------------------------------
Table::Table(table_name name) {
  name_ = name;
  defined_ = false;
}

// -----------------------------------------------------------------------
void Table::defineTable(std::vector<column> cols, primary_key keys){
  vector<column>::iterator each_col;
  for(each_col=cols.begin(); each_col!=cols.end(); each_col++){
    this->addField(each_col->first, each_col->second);
  }
  this->setPrimaryKey(keys);
  this->tableDefined();
}

// -----------------------------------------------------------------------
void Table::tableDefined() {
  defined_ = true;
  BI->registerTable( this );
  LOG(LEV_DEBUG4,"table") << "Table is defined with creation command: " 
                          << this->create();
}

// -----------------------------------------------------------------------
void Table::setPrimaryKey(std::string const pk_string) {
  primary_key pk;
  pk.push_back(pk_string);
  this->setPrimaryKey(pk);
}

// -----------------------------------------------------------------------
void Table::addField(std::string name, std::string data_type) {
  col_names_.push_back(name);
  col_types_.push_back(data_type);
}

// -----------------------------------------------------------------------
void Table::addForeignKey(foreign_key const fk) {
  int key_size = fk.first.size();
  int ref_size = fk.second.second.size();
  if (key_size != ref_size) {
    string err = "Attempted to reference a different foreign key.";
    throw CycException(err);
  }
  foreign_keys_.push_back(fk);
}

// -----------------------------------------------------------------------
string Table::create(){
  // create a table using this table's name
  command cmd("");
  cmd << "CREATE TABLE " << this->name() <<" (";
  // for each entry, add the column name and data type
  // and comma separate entries
  for(int i = 0; i < col_names_.size(); i++) {
    // add the column and data type to the command
    cmd << col_names_.at(i) << " " << col_types_.at(i);
    // if this is the last entry, do not comma separate
    if (i < col_names_.size() - 1) {
      cmd << ", ";
    }
  }
  // add primary keys
  if (primary_key_.size() > 0)
    cmd << ", " << this->p_key();
  // add foreign keys
  if (foreign_keys_.size() > 0)
    cmd << ", " << this->f_keys();
  // close the create table command
  cmd << ");";

  // return a stringified version of the command
  return cmd.str();
}

// -----------------------------------------------------------------------
string Table::stringifyData(data const d){
  command data("");
  string check = "Ss";
  if (d.type().name() == check){
    data << "'" << d << "'";
  } else {
    data << d;
  }
  //LOG(LEV_DEBUG4,"tabled") << "Data has type '" << d.type().name() << "'";
  return data.str();
}

// -----------------------------------------------------------------------
void Table::addRow(row const r){
  command *cmd = new command(""), *cols = new command(""), 
    *values = new command("");
  int nEntries = r.size();
  // collect the column name and values of each entry in the row
  for (int j = 0; j < nEntries; j++){
    (*cols) << r.at(j).first;
    (*values) << stringifyData(r.at(j).second);
    if (j < nEntries-1){
      (*cols) << ", ", (*values) << ", ";
    }
  }
  // create the row insertion command
  (*cmd) << "INSERT INTO " << this->name() << " (" << cols->str() << ") "
         << "VALUES (" << values->str() << ");";
  row_commands_.push_back(cmd);
  LOG(LEV_DEBUG4,"table") << "Added command to row commands: " 
  			  << cmd->str();
  // if we've reached the predefined number of row commands to execute,
  // then inform the BookKeeper as such
  if (nRows() >= BI->rowThreshold())
    BI->tableAtThreshold( this );
}

// -----------------------------------------------------------------------
void Table::updateRow(primary_key_ref const pkref, entry const e){
  // @gidden can we do this without a full pkref or full pkref storage?
  command *cmd = new command("");
  command data("");
  (*cmd) << "UPDATE " << this->name() << " ";
  (*cmd) << "SET " << e.first << "=" << stringifyData(e.second) << " ";
  (*cmd) << "WHERE " << updateRowPK(pkref) << ";";
  row_commands_.push_back(cmd);
  LOG(LEV_DEBUG4, "table") << "Added command to row commands: " << cmd->str();
  // if we've reached the predefined number of row commands to execute,
  // then inform the BookKeeper as such
  if (nRows() >= BI->rowThreshold())
    BI->tableAtThreshold(this);
}

// -----------------------------------------------------------------------
void Table::updateRow(primary_key_ref const pkref, row const r){
  int nEntries = r.size();
  for (int i = 0; i < nEntries; i++){
    this->updateRow(pkref,r.at(i));
  }
}

// -----------------------------------------------------------------------
string Table::updateRowPK(primary_key_ref const pkref){
  command cmd("");
  int nRefs = pkref.size();
  if (nRefs > 0) {
    for (int i = 0; i < nRefs; i++){
      cmd << pkref.at(i).first << "=" << stringifyData(pkref.at(i).second);
      if (i < nRefs-1)
        cmd << " AND ";
    }
  }
  else {
    //throw CycException("Cannot update a row with a non-existant primary key.");
    // @gidden This creates an error in converter tests... not sure why
    cmd << "";
  }
  return cmd.str();
}

// -----------------------------------------------------------------------
string Table::f_keys(){
  // create a the foreign keys statement
  command final_cmd("");
  int nKeys = foreign_keys_.size();
  // if there are no foreign keys, return an empty command
  if (nKeys > 0){
    for (int i = 0; i < nKeys; i++){
      // get key and reference information
      foreign_key fKey = foreign_keys_.at(i);
      foreign_key_ref fKey_ref = fKey.second;
      table_name refTable = fKey_ref.first;

      // construct the foreign key command
      command fKey_cmd("");
      command keys("");      // column names from this table
      command refKeys("");   // column names from the reference table
      
      // collect the column names for this table and the ref table
      int nKey_Cols = fKey.first.size();
      for (int j = 0; j < nKey_Cols; j++){	
	col_name key = fKey.first.at(j);
	col_name refKey = fKey_ref.second.at(j);
	keys << key;
	refKeys << refKey;
	// comma deliminate until last entry
	if (j < nKey_Cols-1){
	  keys << ", ";
	  refKeys << ", ";
	}
      }
      
      // construct the foreign key command
      fKey_cmd << "FOREIGN KEY (" << keys.str() << ") REFERENCES "
	       << refTable << "(" << refKeys.str() << ")";
      // comma deliminate until last entry
      if (i < nKeys -1) {
	fKey_cmd << ", ";
      }

      final_cmd << fKey_cmd.str();
    }
  }
  return final_cmd.str();
}

// -----------------------------------------------------------------------
string Table::p_key(){
  // create a the primary key statement
  command cmd("");
  int nCols = primary_key_.size();
  // if there is no primary key, return an empty command
  if (nCols > 0){
    cmd << "PRIMARY KEY (";
    // for each column, add the column name to the key
    // and comma separate entries
    for (int i = 0; i < primary_key_.size(); i++) {
      cmd << primary_key_.at(i);
      // if this is the last entry, do not comma separate
      if (i < primary_key_.size() - 1) {
        cmd << ", ";
      }
    }
    // close the primary key command
    cmd << ")";
  }
  return cmd.str();
}
