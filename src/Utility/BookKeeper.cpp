// BookKeeper.cpp
// Implements the BookKeeper class
#include "BookKeeper.h"
#include "Timer.h"

#include "hdf5.h"
#include "H5Cpp.h"
#include "H5Exception.h"

BookKeeper* BookKeeper::_instance = 0;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
BookKeeper* BookKeeper::Instance()
{
	// If we haven't created a BookKeeper yet, create and return it.
	if (0 == _instance)
		_instance = new BookKeeper();
	
	return _instance;
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
BookKeeper::BookKeeper() 
{
  // we'd like to call the output database cyclus.h5
  dbName = "cyclus.h5";
  const int dim1 = 1; 
  const int dim2 = LI->getNumFacilities();;

  double data[dim1][dim2];
  for (int i=0; i<dim2; i++){
    //fac = LI->getFacilityByID[i];
    data[0][i] = i;
  }

  hsize_t dims[1];
  dims[0]= dim1;
  dims[1]= dim2;
  hsize_t rank = 2;

  try{
    // create database. If it already exits, H5F_ACC_TRUNC erases all 
    // data previously stored in the file.
    myDB = new H5File( dbName , H5F_ACC_TRUNC );

    // create groups for the input and output data, respectively
    Group* outGroup = new Group( myDB->createGroup("/output"));
    Group* inGroup = new Group( myDB->createGroup("/input"));

    // create a basic dataset to hold model information
    dbIsOpen = false;

    DataSpace dataspace = DataSpace(rank , dims );

    DataType datatype = DataType(PredType::NATIVE_DOUBLE);

    DataSet dataset = myDB->createDataSet("/output/test", datatype, dataspace) ; 

    dataset.write(data, datatype);

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
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//DSetPropList* BookKeeper::dsPropList(){
  // nothing doing
//};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DataSpace* BookKeeper::homoDataSpace(Group* grp, string name, map< int, pair< string, PredType > > typemap){

  // each dimension needs to use it's knowledge of its predtype 
  // to define the dataspace type
  //
  // this is not the same as with compound types. 
  //
  //
  // the dimensions are the first entries of the map
  vector<hsize_t> dims;

  map<int,pair<string, PredType> >::const_iterator iter=typemap.begin();

  int i = 0;
 
  while (iter != typemap.end()){
    dims[i]=iter->first;
    i++;
    iter++;
  }
  // the length of this map is the rank
  int rank=i;

  // I'm hoping predtypes are already made explicit in the H5CPP api
  DataSpace* toRet = new DataSpace(( rank, dims[1],dims[2]));

  return toRet;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DataSet* BookKeeper::fillDataSet(DataSpace* ds, vector<int> data)
{
 // DataSet* toRet;
 // toRet = new DataSet(DataSet(PredType::NATIVE_INT));
 // toRet->write(data);
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



