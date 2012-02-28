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
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool BookKeeper::fexists(const char *filename) {
  std::ifstream ifile(filename);
  return ifile;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void BookKeeper::createDB(std::string name) {
  dbName_ = name;
  try{

    // construct output file path
    std::string out_path = ENV->checkEnv("CYCLUS_OUT_DIR");
    std::string db_path = out_path + "/" + name;

    // if the file already exists, delete it
    if( fexists( db_path.c_str() ) )
      remove( db_path.c_str() );

    // create database. 
    db_ = new Database(name);
    if ( dbExists() ) {
      dbIsOpen_ = true;
      LOG(LEV_DEBUG3,"DBInfo") << "Successfully created the output database" 
                               << " at file location: " << db_path;
    }
  }
  catch ( CycException& error ) {
    throw CycException( std::string(error.what()) );
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
void BookKeeper::turnOffLogging() {
  logging_on_ = false;
  if ( db_->nTables() > 0 ) {
    std::string err = 
      "Logging can not be turned off once a table has already been created.";
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
void BookKeeper::tableAtThreshold(table_ptr t) {
  if ( loggingIsOn() ) {
    db_->writeRows(t);
    t->flush();
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool BookKeeper::isOpen() {
  if (!dbExists() && dbIsOpen_) {
    std::string err = 
      "The BookKeeper reports that the Database does NOT exist but is OPEN.";
    throw CycException(err);
    return false;
  }
  else
    return dbIsOpen_;
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
  db_->close();
}
