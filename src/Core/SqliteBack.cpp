
#include "SqliteBack.h"

#include "CycException.h"
#include "Logger.h"

#include <iostream>
#include <fstream>

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
SqliteBack::SqliteBack(std::string filename){
  db_ = NULL;
  isOpen_ = false;
  path_ = filename;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
SqliteBack::~SqliteBack() { }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
std::string SqliteBack::name() {
  return path_;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void SqliteBack::notify(EventList evts) {
  for (EventList::iterator it = evts.begin(); it != evts.end(); it++) {
    if (! tableExists(*it) ) {
      createTable(*it);
    }
    writeEvent(*it);
  }
  flush();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void SqliteBack::close() {
  if ( isOpen_ ) {
    flush();
    if (sqlite3_close(db_) == SQLITE_OK) {
      isOpen_ = false;
    } else {
      throw CycIOException("Failed to close database: " + path_);
    }
  } else {
    throw CycIOException("Cannot close a closed database: " + path_);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool SqliteBack::fexists(const char *filename) {
  ifstream ifile(filename);
  return ifile;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void SqliteBack::open() {
  if (isOpen_) {
    return;
  }

  // if the file already exists, delete it
  ifstream ifile(path_);
  if( ifile ) {
    remove( path_.c_str() );
  }

  if(sqlite3_open(path_.c_str(), &db_) == SQLITE_OK) {
    isOpen_ = true;
  } else {
    throw CycIOException("Unable to create/open database " + path_); 
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void SqliteBack::createTable(event_ptr e){
  std::string name = e->name();
  tbl_names_.push_back(name);

  std::string cmd = "CREATE TABLE " + name + " (";

  ValMap vals = e.vals();
  for (ValMap::iterator it = vals.begin(); it != vals.end(); it++) {
    if (it + 1 != vals.end()) {
      cmd += it->first + " " + valType(it->second) + ", ";
    } else {
      cmd += it->first + " " + valType(it->second);
    }
  }
  cmd += ");";
  cmds_.push_back(cmd);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
std::string SqliteBack::valType(boost::any v) {
  std::type_info t = v.type();
  if (t == typeid(int)) {
    return "INTEGER"
  } else if (t == typeid(float)) {
    return "REAL"
  } else if (t == typeid(double)) {
    return "REAL"
  } else if (t == typeid(std::string)) {
    return "VARCHAR(128)"
  }
  return "VARCHAR(128)"
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
bool SqliteBack::tableExists(event_ptr e) {
  std::string nm = e->name();
  if (find(tbl_names_.begin(), tbl_names_.end(), nm) != tbl_names_.end()) {
    return true;
  }
  return false
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void SqliteBack::executeSQL(std::string cmd){  
  open();

  sqlite3_stmt *statement;
  int check_query = 
    sqlite3_prepare_v2(db_, cmd.c_str(), -1, &statement, 0);
  if(check_query == SQLITE_OK) {
    int result = sqlite3_step(statement);
    sqlite3_finalize(statement);
  }
  // collect sqlite errors
  string error = sqlite3_errmsg(db_);
  if(error != "not an error") {
    throw CycIOException("SQL error: " + cmd + " " + error);
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void SqliteBack::writeEvent(event_ptr e){
  std::stringstream cols, vals, cmd;
  ValMap vals = e.vals();
  for (ValMap::iterator it = vals.begin(); it != vals.end(); it++) {
    cols << it->first;
    vals << it->second;
    if (it != vals.end()-1){
      cols << ", ";
      vals << ", ";
    }
  }

  cmd << "INSERT INTO " << e->name() << " (" << cols.str() << ") "
         << "VALUES (" << values.str() << ");";
  cmds_.push_back(cmd.str());
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void SqliteBack::flush(){
  for (StrList::iterator it = cmds_.begin(); it != cmds_.end(); it++) {
    executeSQL(*it);
  }
}

