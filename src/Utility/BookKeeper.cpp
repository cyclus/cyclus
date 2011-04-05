// BookKeeper.cpp
// Implements the BookKeeper class
#include "BookKeeper.h"
#include "Timer.h"

#include "hdf5.h"
#include "H5Cpp.h"
#include "H5Exception.h"
//#include <stdio.h>
//#include <stdlib.h>
//#include <iostream>
//#include <fstream>
//#include <sstream>
//#include <cstring>

BookKeeper* BookKeeper::_instance = 0;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
BookKeeper* BookKeeper::Instance()
{
	// If we haven't created a BookKeeper yet, create and return it.
	if (0 == _instance)
		_instance = new BookKeeper();
	
	return _instance;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
BookKeeper::BookKeeper() 
{
  // we'd like to call the output database cyclus.h5
  dbName = "cyclus.h5";

  try{
    // create database. If it already exits, H5F_ACC_TRUNC erases all 
    // data previously stored in the file.
    myDB = new H5File( dbName , H5F_ACC_TRUNC );

    // create groups for the input and output data, respectively
    Group* outGroup = new Group( myDB->createGroup("/output"));
    Group* inGroup = new Group( myDB->createGroup("/input"));

    // create a basic dataset to hold model information
    dbIsOpen = false;

    delete outGroup;
    delete inGroup;
  }
  catch( FileIException error )
  {
    error.printError();
  }
  catch( GroupIException error )
  {
    error.printError();
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
H5File* BookKeeper::getDB()
{
	return myDB;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BookKeeper::closeDB()
{
	// Try to close it.
  try{
    myDB->close();
    dbIsOpen = false;
  }
  // catch failure caused by the H5File operations
  catch( FileIException error )
  {
     error.printError();
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BookKeeper::openDB()
{
	// If the database is already open, throw an exception; the caller probably 
	// doesn't realize this.
  try{
    H5File* memDB = new H5File( dbName , H5F_ACC_RDWR );  
  }
  catch( FileIException error )
  {
    error.printError();
  }
	
	// Create datasets to store the information we want? 
	
	// Store the handle to it.
	dbIsOpen = true;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//	DSetCreatPropList* BookKeeper::createPropList(){};
	/**
	 * Create a property list for a dataset and set up fill values.
	 */

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//	DataSpace* BookKeeper::createDataSpace(){};
	/**
	 * Creates a dataspace for the dataset in the file....
	 */

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//	void BookKeeper::createDataSet(){};
	/**
	 * you'll need a dataspace... 
	 */

}



