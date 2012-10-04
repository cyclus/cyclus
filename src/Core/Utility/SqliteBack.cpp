
#include "SqliteBack.h"

#include "CycException.h"
#include "Logger.h"
#include "Event.h"

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

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void SqliteBack::open() {
  if (isOpen_) {
    return;
  }

  // if the file already exists, delete it
  std::ifstream ifile(path_.c_str());
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

  ValMap vals = e->vals();
  ValMap::iterator it = vals.begin();
  while (true) {
    cmd += it->first + " " + valType(it->second);
    ++it;
    if (it == vals.end()) {
      break;
    }
    cmd += ", ";
  }

  cmd += ");";
  cmds_.push_back(cmd);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
std::string SqliteBack::valType(boost::any v) {
  if (v.type() == typeid(int)) {
    return "INTEGER";
  } else if (v.type() == typeid(float)) {
    return "REAL";
  } else if (v.type() == typeid(double)) {
    return "REAL";
  } else if (v.type() == typeid(std::string)) {
    return "VARCHAR(128)";
  }
  return "VARCHAR(128)";
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
bool SqliteBack::tableExists(event_ptr e) {
  std::string nm = e->name();
  if (find(tbl_names_.begin(), tbl_names_.end(), nm) != tbl_names_.end()) {
    return true;
  }
  return false;
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
  std::string error = sqlite3_errmsg(db_);
  if(error != "not an error") {
    throw CycIOException("SQL error: " + cmd + " " + error);
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void SqliteBack::writeEvent(event_ptr e){
  std::stringstream cs, vs, cmd;
  ValMap vals = e->vals();
  ValMap::iterator it = vals.begin();
  while (true) {
    cs << it->first;
    vs << valData(it->second);
    ++it;
    if (it == vals.end()) {
      break;
    }
    cs << ", ";
    vs << ", ";
  }

  cmd << "INSERT INTO " << e->name() << " (" << cs.str() << ") "
         << "VALUES (" << vs.str() << ");";
  cmds_.push_back(cmd.str());
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
std::string SqliteBack::valData(boost::any v) {
  std::stringstream ss;
  if (v.type() == typeid(int)) {
    ss << boost::any_cast<int>(v);
  } else if (v.type() == typeid(float)) {
    ss << boost::any_cast<float>(v);
  } else if (v.type() == typeid(double)) {
    ss << boost::any_cast<double>(v);
  } else if (v.type() == typeid(std::string)) {
    ss << boost::any_cast<std::string>(v);
  } else {
    CLOG(LEV_ERROR) << "attempted to record unsupported type in backend "
      << name();
  }
  return ss.str();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void SqliteBack::flush(){
  for (StrList::iterator it = cmds_.begin(); it != cmds_.end(); it++) {
    executeSQL(*it);
  }
}

