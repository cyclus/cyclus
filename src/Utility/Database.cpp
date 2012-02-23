#include "Database.h"

#include <iostream>
#include <string>
#include <sqlite3.h>

#include "CycException.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Database::Database(std::string filename){
  database_ = NULL;
  open(filename);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Database::~Database(){}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool Database::open(std::string filename){
  if(sqlite3_open(filename.c_str(), &database_) == SQLITE_OK)
    return true;
  else 
    throw CycIOException("Unable to open database " + filename); 
  return false;   
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
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

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Database::createTable(Table* t){
  // declare members
  sqlite3_stmt *statement;
  std::string query = t->create();
  this->issueCommand(query);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Database::writeRows(Table* t){
  // declare members
  std::string query = t->writeRows();
  this->issueCommand(query);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
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
  

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Database::close(){
  sqlite3_close(database_);   
}
