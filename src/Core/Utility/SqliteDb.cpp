// SqliteDb.cpp

#include "SqliteDb.h"

#include "CycException.h"
#include "Logger.h"
#include "Event.h"

#include <fstream>

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
SqliteDb::SqliteDb(std::string path) {
  db_ = NULL;
  isOpen_ = false;
  path_ = path;
  overwrite_ = false;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
SqliteDb::~SqliteDb() {
  if ( isOpen_ ) {
    close();
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void SqliteDb::overwrite() {
  overwrite_ = true;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void SqliteDb::close() {
  if ( isOpen_ ) {
    if (sqlite3_close(db_) == SQLITE_OK) {
      isOpen_ = false;
    } else {
      throw CycIOException("Failed to close database: " + path_);
    }
  } else {
    throw CycIOException("Cannot close a closed database: " + path_);
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void SqliteDb::open() {
  if (isOpen_) {
    return;
  }

  // if the file already exists, delete it
  if (overwrite_) {
    std::ifstream ifile(path_.c_str());
    if( ifile ) {
      remove( path_.c_str() );
    }
  }

  if(sqlite3_open(path_.c_str(), &db_) == SQLITE_OK) {
    isOpen_ = true;
  } else {
    throw CycIOException("Unable to create/open database " + path_); 
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void SqliteDb::execute(std::string sql) {
  open();

  sqlite3_stmt *statement;
  int check_query = 
    sqlite3_prepare_v2(db_, sql.c_str(), -1, &statement, 0);
  if(check_query == SQLITE_OK) {
    int result = sqlite3_step(statement);
    sqlite3_finalize(statement);
  }

  // collect sqlite errors
  std::string error = sqlite3_errmsg(db_);
  if(error != "not an error") {
    throw CycIOException("SQL error: " + sql + " " + error);
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
std::vector<StrList> SqliteDb::query(std::string sql) {
  open();

  sqlite3_stmt *statement;
  std::vector<StrList> results;
  
  // query the database
  int check_query = 
    sqlite3_prepare_v2(db_, sql.c_str(), -1, &statement, 0);
  if(check_query == SQLITE_OK){
    int cols = sqlite3_column_count(statement);
    int result = 0;
    while(true){
      result = sqlite3_step(statement);
      if(result != SQLITE_ROW){
        break;
      }

      StrList values;
      for(int col = 0; col < cols; col++){
        std::string  val;
        char * ptr = (char*)sqlite3_column_text(statement, col);
        if(ptr) {
          val = ptr;
        } else {
          val = "";
        }
        values.push_back(val);  // now we will never push NULL
      }
      results.push_back(values);
    } 
    sqlite3_finalize(statement);
  }

  // collect errors
  std::string error = sqlite3_errmsg(db_);
  if(error != "not an error") {
    throw CycIOException("SQL error: " + sql + " " + error);
  }

  return results;  
}

