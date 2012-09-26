
#include "SqliteBack.h"

#include "Table.h"
#include "CycException.h"
#include "Logger.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sqlite3.h>

using namespace std;

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
SqliteBack::SqliteBack(std::string filename){
  db_ = NULL;
  isOpen_ = false;
  name_ = filename;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
SqliteBack::~SqliteBack() { }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void SqliteBack::notify(EventList evts) {
  for (EventList::iterator it = evts.begin(); it != evts.end(); it++) {
    if (! tableExists(*it) ) {
      createTable(*it);
    }
    writeRow(*it);
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void SqliteBack::close() {
  if ( isOpen_ ) {
    if (sqlite3_close(db_) == SQLITE_OK) {
      isOpen_ = false;
      return;
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
  std::string name = tableName(e);
  tbl_names_.push_back(name);

  std::stringstream cmd;
  cmd << "CREATE TABLE " << name <<" (";

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
  // close the create table command
  cmd << ");";

  cmds_.push_back(cmd.str());
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
std::string tableName(event_ptr e) {
  Model* m = e->creator();
  return m->modelImpl() + "_" + m->ID() + "_" + e->group();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
bool SqliteBack::tableExists(event_ptr e) {
  std::string nm = tableName(e);
  if (find(tbl_names_.begin(), tbl_names_.end(), nm) != tbl_names_.end()) {
    return true;
  }
  return false
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void SqliteBack::executeSQL(std::string cmd){  
  if ( !isOpen_ ) {
    throw CycIOException("Tried to issue command to closed table: " + name_);
  }

  sqlite3_stmt *statement;
  int check_query = 
    sqlite3_prepare_v2(db_, cmd.c_str(), -1, &statement, 0);
  if(check_query == SQLITE_OK) {
    int result = sqlite3_step(statement);
    sqlite3_finalize(statement);
  }
  // collect errors
  string error = sqlite3_errmsg(db_);
  if(error != "not an error") {
    throw CycIOException("SQL error: " + cmd + " " + error);
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void SqliteBack::writeRow(event_ptr e){
  // write each row in the Table's row commands
  int nRows = t->nRows();
  for (int i = 0; i < nRows; i++){
    string cmd_str = t->row_command(i)->str();
    this->cmds_.push_back(cmd_str);
    LOG(LEV_DEBUG4,"db") << "Issued writeRows command to table: " 
             << t->name() << " with the command being " 
             << cmd_str;
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void SqliteBack::flush(){
  for (StrList::iterator it = cmds_.begin(); it != cmds_.end(); it++) {
    executeSQL(*it);
  }
}

