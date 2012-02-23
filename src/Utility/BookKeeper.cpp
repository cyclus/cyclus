// BookKeeper.cpp
// Implements the BookKeeper class

#include "BookKeeper.h"

#include <string>
#include <iostream>
#include <fstream>
#include <stdlib.h>

#include "Database.h"
#include "Table.h"

BookKeeper* BookKeeper::instance_ = 0;
  

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
BookKeeper* BookKeeper::Instance() {
  // If we haven't created a BookKeeper yet, create and return it.
  if (0 == instance_){
    instance_ = new BookKeeper();  
  }
  return instance_;
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
BookKeeper::BookKeeper() {
  dbIsOpen_ = false;
  dbExists_ = false;
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
bool BookKeeper::fexists(const char *filename) {
  std::ifstream ifile(filename);
  return ifile;
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
void BookKeeper::createDB(std::string name) {
  dbName_ = name;
  try{
    // get database name and location
    char* oPath = getenv("CYCLUS_OUTPUT_DIR");
    if (oPath==NULL) {
      std::string err = "Cyclus output path - envrionment variable: " +
	"CYCLUS_OUTPUT_DIR - not defined" ;
      throw CycIOError(err);
    }

    // construct output file path
    std::string out_path(oPath);
    std::string db_path = out_path + "/" + name;

    // if the file already exists, delete it
    if( fexists( db_path.c_str() ) )
      remove( db_path.c_str() );

    // create database. 
    db_ = new Database(name);
    dbIsOpen_ = true; 
    dbExists_ = true;
  } catch( Exception error ) {
    error.printError();
  }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BookKeeper::registerTable(Table* t) {
  db_->registerTable(t);
  db_->createTable(t);
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BookKeeper::tableAtThreshold(Table* t) {
  db_->writeRows(t);
  t->flush();
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BookKeeper::closeDB() {
  // have the database print and remaining commands
  for (int i = 0; i < db_->nTables(); i++) {
    Table* t = db_->tablePtr(i);
    if (t->nRows() > 0)
      this->tableAtThreshold(t);
  }
  // close the db
  db_->close();
}
