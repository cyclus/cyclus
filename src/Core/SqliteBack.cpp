
#include "SqliteBack.h"

#include "CycException.h"
#include "Logger.h"

#include <iostream>
#include <fstream>

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
SqliteBack::SqliteBack(std::string filename){
  db_ = NULL;
  isOpen_ = false;
  name_ = filename;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
SqliteBack::~SqliteBack() { }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
std::string SqliteBack::name() {
  return "Sqlite-backend";
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
      throw CycIOException("Error closing existing database: " + name_);
    }
  } else {
    throw CycIOException("Trying to close an already-closed database: " + name_);
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

  if ( dbExists() ) {
    if(sqlite3_open(name_.c_str(), &db_) == SQLITE_OK) {
      isOpen_ = true;
      return;
    } else {
      throw CycIOException("Unable to open database " + name_); 
    }
  } else {
    throw CycIOException("Trying to open a non-existant SqliteBack file"); 
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void SqliteBack::createTable(event_ptr e){
  Model* m = e->creator();
  std::string name = e->name();
  tbl_names_.push_back(name);

  std::stringstream cmd;
  cmd << "CREATE TABLE " << name <<" (";

  for(int i = 0; i < col_names_.size(); i++) {
    cmd << col_names_.at(i) << " " << col_types_.at(i);
    if (i < col_names_.size() - 1) {
      cmd << ", ";
    }
  }
  cmd << ");";

  cmds_.push_back(cmd.str());
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

  cmd << "INSERT INTO " << name() << " (" << cols.str() << ") "
         << "VALUES (" << values.str() << ");";
  cmds_.push_back(cmd.str());
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void SqliteBack::flush(){
  for (StrList::iterator it = cmds_.begin(); it != cmds_.end(); it++) {
    executeSQL(*it);
  }
}

