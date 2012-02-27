#include "Database.h"

#include <iostream>
#include <string>
#include <sqlite3.h>

#include "Table.h"
#include "CycException.h"
#include "Logger.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
Database::Database(std::string filename){
  database_ = NULL;
  exists_ = false;
  name_ = filename;
  if ( open(filename) ) 
    exists_ = true;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
Database::~Database(){}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
bool Database::open(std::string filename){
  if(sqlite3_open(filename.c_str(), &database_) == SQLITE_OK)
    return true;
  else 
    throw CycIOException("Unable to open database " + filename); 
  return false;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
bool Database::dbExists() {
  if (this != NULL)
    return exists_;
  else
    return false;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void Database::registerTable(table_ptr t) {
  if ( dbExists() )
    tables_.push_back(t);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void Database::createTable(table_ptr t){
  // note that we first check for if the data base exists
  // this is due to the fact that there is not good testing for bookkeeping
  // i.e., we are trying to automate as much as possible, so tests
  // currently try to access a db that is never instantiated
  if ( dbExists() ) {
    bool tExists = tableExists(t);
    if (tExists) {
      // declare members
      std::string query = t->create();
      this->issueCommand(query);
    }
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
bool Database::tableExists(table_ptr t) {
  // make sure the table exists before it is accessed
  bool isPresent = false;
  std::string err;
  
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
	(std::find(tables_.begin(), tables_.end(), t) != tables_.end());
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
void Database::issueCommand(std::string cmd){  
  sqlite3_stmt *statement;
  // query the database
  int check_query = 
    sqlite3_prepare_v2(database_, cmd.c_str(), -1, &statement, 0);
  if(check_query == SQLITE_OK) {
    int result = sqlite3_step(statement);
    sqlite3_finalize(statement);
  }
  // collect errors
  std::string error = sqlite3_errmsg(database_);
  if(error != "not an error") 
    throw CycIOException("SQL error: " + cmd + " " + error);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void Database::writeRows(table_ptr t){
  // note that we first check for if the data base exists
  // this is due to the fact that there is not good testing for bookkeeping
  // i.e., we are trying to automate as much as possible, so tests
  // currently try to access a db that is never instantiated
  if ( dbExists() ) {
    bool exists = tableExists(t);
    if (exists) {
      // write each row in the Table's row commands
      int nRows = t->nRows();
      for (int i = 0; i < nRows; i++){
	std::string cmd_str = t->row_command(i)->str();
	this->issueCommand(cmd_str);
	LOG(LEV_DEBUG5,"db") << "Issued writeRows command to table: " 
			     << t->name() << " with the command being " 
			     << cmd_str;
      }
    }
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
query_result Database::query(std::string query){
  // declare members
  sqlite3_stmt *statement;
  query_result results;
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
	  std::string  val;
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
  std::string error = sqlite3_errmsg(database_);
  if(error != "not an error") 
    throw CycIOException("SQL error: " + query + " " + error);
  
  return results;  
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void Database::close(){
  sqlite3_close(database_);   
}
