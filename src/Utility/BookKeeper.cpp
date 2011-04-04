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
BookKeeper::BookKeeper() {
	dbIsOpen = false;
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
  }
  // catch failure caused by the H5File operations
  catch( FileIException error )
  {
     error.printError();
  }
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BookKeeper::openDB(char* name)
{
	// If the database is already open, throw an exception; the caller probably 
	// doesn't realize this.
  try{
    H5File* myDB = new H5File( name , H5F_ACC_RDWR ); 
  }
  catch( FileIException error )
  {
    error.printError();
  }

  // Delete file "cyclus.h5" if it exists. 
	finDBName = string("./cyclus.h5");
	remove(finDBName.c_str());
	
	// Create datasets to store the information we want? 
	
	// Store the handle to it.
	dbIsOpen = true;
}
