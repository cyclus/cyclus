#include "Database.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sqlite3.h>

#include "Table.h"
#include "CycException.h"
#include "Logger.h"

using namespace std;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
Database::Database(string filename){
  database_ = NULL;
  exists_ = true;
  isOpen_ = false;
  name_ = filename;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
Database::Database(string filename, string file_path){
  database_ = NULL;
  exists_ = true;
  isOpen_ = false;
  name_ = filename;
  path_ = file_path;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool Database::fexists(const char *filename) {
  ifstream ifile(filename);
  return ifile;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
string Database::path() {
  string full_path = "";
  if ( !path_.empty() ) {
    full_path += path_ + "/";
  }
  return full_path;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
bool Database::open() {
  if ( dbExists() ) {
    string path_to_file = path() + name_;
    if(sqlite3_open(path_to_file.c_str(), &database_) == SQLITE_OK) {
      isOpen_ = true;
      return true;
    }
    else {
      throw CycIOException("Unable to open database " + path_to_file); 
    }
  } // end if ( dbExists() )
  else {
    throw CycIOException("Trying to open a non-existant Database"); 
  }
  return false;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
bool Database::close() {
  if ( isOpen() ) {
    if (sqlite3_close(database_) == SQLITE_OK) {
      isOpen_ = false;
      return true;
    }
    else {
      throw CycIOException("Error closing existing database: " + name_);
      return false;
    }
  } // endif ( isOpen() )
  else {
    throw CycIOException("Trying to close an already-closed database: " + name_);
    return false;
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
bool Database::isOpen() {
  string err;
  if ( dbExists() )
    return isOpen_;
  else {
    err = "Database " + name_ + " is not open because it does not exist.";
    throw CycIOException(err);
  }
  return false;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
bool Database::dbExists() {
  if (this != NULL) {
    return true;
  }
  else {
    return false;
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void Database::registerTable(table_ptr t) {
  if ( dbExists() ) {
    tables_.push_back(t);
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void Database::removeTable(table_ptr t) {
  if ( dbExists() ) {
    tables_.erase(find(tables_.begin(), tables_.end(), t));
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void Database::createTable(table_ptr t){
  if ( isOpen() ) {
    bool tExists = tableExists(t);
    if (tExists) {
      // declare members
      string query = t->create();
      this->issueCommand(query);
    }
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
bool Database::tableExists(table_ptr t) {
  // make sure the table exists before it is accessed
  bool isPresent = false;
  string err;
  
  // case: the database is not null
  if ( dbExists() ) {
    // case: the table pointer is null
    if ( t == NULL ) 
      err = "Database " + this->name() 
	+ " was asked to interact with a non-existant table.";
    // case: the table pointer is NOT null but the table is not created
    else if ( !t->defined() )
      err = "Database " + this->name() 
	+ " was asked to interact with a existant, but non-defined table.";
    // case: table is defined, search for it
    else {
      isPresent = 
	(find(tables_.begin(), tables_.end(), t) != tables_.end());
      if (!isPresent)
	err = "Table: " + t->name() 
	  + "  is not registered with Database " + this->name() + ".";
    }
  }
  // case: the database is null
  else
    err = "An attempt was made to interact with a non existant Database.";

  // found, return true
  if(isPresent)
    return true;
  else {
    // not found, throw an error and return false
    throw CycIOException(err);
    return false;
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void Database::issueCommand(string cmd){  
  if ( isOpen() ) {
    sqlite3_stmt *statement;
    // query the database
    int check_query = 
      sqlite3_prepare_v2(database_, cmd.c_str(), -1, &statement, 0);
    if(check_query == SQLITE_OK) {
      int result = sqlite3_step(statement);
      sqlite3_finalize(statement);
    }
    // collect errors
    string error = sqlite3_errmsg(database_);
    if(error != "not an error") 
      throw CycIOException("SQL error: " + cmd + " " + error);
  }
  else
    throw CycIOException("Tried to issue command to closed table: " + name_);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void Database::writeRows(table_ptr t){
  if ( isOpen() ) {
    bool exists = tableExists(t);
    if (exists) {
      // write each row in the Table's row commands
      int nRows = t->nRows();
      for (int i = 0; i < nRows; i++){
	string cmd_str = t->row_command(i)->str();
	this->issueCommand(cmd_str);
	LOG(LEV_DEBUG4,"db") << "Issued writeRows command to table: " 
			     << t->name() << " with the command being " 
			     << cmd_str;
      }
    }
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void Database::flush(table_ptr t){
  if ( isOpen() ) {
    bool exists = tableExists(t);
    if (exists) {
      t->flush();
    }
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
query_result Database::query(string query){
  // declare members
  sqlite3_stmt *statement;
  query_result results;
  
  if ( isOpen() ) {
    // query the database
    int check_query = 
      sqlite3_prepare_v2(database_, query.c_str(), -1, &statement, 0);
    if(check_query == SQLITE_OK){
      int cols = sqlite3_column_count(statement);
      int result = 0;
      while(true){
	result = sqlite3_step(statement);
	// access the rows
	if(result == SQLITE_ROW){
	  query_row values;
	  for(int col = 0; col < cols; col++){
	    string  val;
	    char * ptr = (char*)sqlite3_column_text(statement, col);
	    if(ptr)
	      val = ptr;
	    else 
	      val = "";
	    values.push_back(val);  // now we will never push NULL
	  }
	  results.push_back(values);
	}
	else
	  break;  
      } 
      sqlite3_finalize(statement);
    }
    // collect errors
    string error = sqlite3_errmsg(database_);
    if(error != "not an error") 
      throw CycIOException("SQL error: " + query + " " + error);
  }
  else
    throw CycIOException("Attempted to query the closed database: " + name_);
  return results;  
}
