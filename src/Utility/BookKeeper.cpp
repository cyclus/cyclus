// BookKeeper.cpp
// Implements the BookKeeper class

#include "BookKeeper.h"

#include <string>
#include <iostream>
#include <fstream>
#include <stdlib.h>

#include "Database.h"
#include "Table.h"
#include "CycException.h"
#include "Env.h"

#define ROW_THRESHOLD 5;

using namespace std;

BookKeeper* BookKeeper::instance_ = 0;
bool BookKeeper::logging_on_ = true;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
BookKeeper* BookKeeper::Instance() {
  // If we haven't created a BookKeeper yet, create and return it.
  if (0 == instance_){
    instance_ = new BookKeeper();  
  }
  return instance_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
BookKeeper::BookKeeper() {
  dbIsOpen_ = false;
  db_ = NULL;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void BookKeeper::createDB(){
  createDB("cyclus.sqlite", Env::getBuildPath());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void BookKeeper::createDB(file_path fpath){
  createDB("cyclus.sqlite", fpath);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void BookKeeper::createDB(std::string name, file_path fpath) {
  dbName_ = name;

  try{
    // construct output file path
    string db_path = fpath + "/" + name;

    // create database. 
    db_ = new Database(name,fpath);

    // if the file already exists, delete it
    if( db_->fexists( db_path.c_str() ) ) {
      remove( db_path.c_str() );
    }

    db_->open();
    if ( dbExists() ) {
      dbIsOpen_ = true;
      LOG(LEV_DEBUG3,"DBInfo") << "Successfully created the output database" 
                               << " at file location: " << db_path;
    }
  }
  catch ( CycException& error ) {
    throw CycException( string(error.what()) );
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool BookKeeper::dbExists() {
  if ( db_ == NULL)
    return false;
  else
    return db_->dbExists();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool BookKeeper::loggingIsOn() {
  if ( dbExists() )
    return logging_on_;
  else
    return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BookKeeper::turnLoggingOn() {
  logging_on_ = true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BookKeeper::turnLoggingOff() {
  logging_on_ = false;
  if ( nTables() > 0 ) {
    string err = "Logging can not be turned off once a table has already been created.";
    throw CycException(err);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BookKeeper::registerTable(table_ptr t) {
  if ( loggingIsOn() ) {
    db_->registerTable(t);
    db_->createTable(t);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BookKeeper::removeTable(table_ptr t) {
  if ( loggingIsOn() ) {
    db_->removeTable(t);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int BookKeeper::nTables() {
  if ( dbExists() ){
    return db_->nTables();
  }
  else {
    return 0;
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int BookKeeper::rowThreshold() {
  return ROW_THRESHOLD;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BookKeeper::tableAtThreshold(table_ptr t) {
  if ( loggingIsOn() ) {
    db_->writeRows(t);
    db_->flush(t);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool BookKeeper::dbIsOpen() {
  if (!dbExists() && dbIsOpen_) {
    string err = 
      "The BookKeeper reports that the Database does NOT exist but is OPEN.";
    throw CycException(err);
    return false;
  }
  else {
    return dbIsOpen_;
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BookKeeper::openDB() {
  if ( !dbIsOpen() ){
    dbIsOpen_ = db_->open();
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BookKeeper::closeDB() {
  // have the database print and remaining commands
  for (int i = 0; i < db_->nTables(); i++) {
    table_ptr t = db_->tablePtr(i);
    if (t->nRows() > 0)
      this->tableAtThreshold(t);
  }
  // close the db
  dbIsOpen_ = !db_->close();
}
