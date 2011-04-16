// BookKeeper.cpp
// Implements the BookKeeper class
#include "BookKeeper.h"
#include "Timer.h"

#include "hdf5.h"
#include "H5Cpp.h"
#include "H5Exception.h"
#include "GenException.h"

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
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BookKeeper::createDB(){
  createDB("cyclus.h5");
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BookKeeper::createDB(string name){
  dbName = name;

  try{
    // create database. If it already exits, H5F_ACC_TRUNC erases all 
    // data previously stored in the file.
    myDB = new H5File( name , H5F_ACC_TRUNC );
    dbIsOpen = true; // use the H5 function for this.. !!! KDHFLAG

    // create groups for the input and output data, respectively
    Group* outGroup = new Group( myDB->createGroup("/output"));
    Group* inGroup = new Group( myDB->createGroup("/input"));

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
DataSet BookKeeper::createDataSet(hsize_t rank, hsize_t* dims, DataType type, string dsName){
  DataSet dataset;
  try{
    // create the dataspace from rank and dimension information
    DataSpace dataspace = DataSpace(rank , dims );

    // create a dataset to match the dataspace
    dataset = this->getDB()->createDataSet(dsName, type, dataspace) ; 

  }
  catch( FileIException error )
  {
    error.printError();
  }
  catch( DataSetIException error )
  {
    error.printError();
  }
  catch( GroupIException error )
  {
    error.printError();
  }
  return dataset;
};

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
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
H5File* BookKeeper::getDB()
{
	return myDB;
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BookKeeper::openDB()
{
  //if(dbIsOpen==false){
	// If the database is already open, throw an exception; the caller probably 
	// doesn't realize this.
  //  try{ 
  //    this->getDB()->reOpen();  
  //  }
  //  catch( FileIException error )
  //  {
  //    error.printError();
  //  }
 // }
	
	//dbIsOpen = true;
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BookKeeper::closeDB()
{
  if(dbIsOpen==true){
    // Try to close it.
    try{
      this->getDB()->close();
      dbIsOpen = false;
    }
    // catch failure caused by the H5File operations
    catch( FileIException error )
    {
      error.printError();
    }
  }
  else
    throw GenException("Tried to close a database that was not open."); 
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool BookKeeper::isGroup(string grp)
{
  bool toRet;
  toRet = false;
  hsize_t nObs = this->getDB()->getNumObjs();
  string testname;
  for( hsize_t i; i< nObs ; i++ ){ 
    testname = BI->getDB()->getObjnameByIdx(i);
    if( testname == grp )
      toRet = true ;
  }
  return toRet;
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BookKeeper::writeData(intData1d data, string dsname){ 

  hsize_t nrows = data.size(); 

  hsize_t dims[1];
  dims[0]=nrows;
  hsize_t rank = 1;

  DataType type = DataType(PredType::NATIVE_INT);
  DataSet dataset =  this->createDataSet(rank, dims, type, dsname);

  // create a file dataspace
  DataSpace filespace = dataset.getSpace();

  // create a memory dataspace
  DataSpace memspace = DataSpace( rank, dims );

  // select the whole dataset as the memory dataspace
  memspace.selectAll();
  
  // select the whole dataset as the memory dataspace
  filespace.selectAll();

  // the data needs to be an array - is this memcpy ugly?
  double dat_array[nrows];
  memcpy( dat_array, &data[0], sizeof( int ) * data.size() );

  // write it
  dataset.write(dat_array, type, memspace, filespace );
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BookKeeper::writeData(intData2d data, string dsname){

  hsize_t nrows = data.size(); 
  hsize_t ncols = data[0].size(); 

  hsize_t dims[2];

  dims[0]=nrows;
  dims[1]=ncols;
  hsize_t rank = 2;

  DataType type = DataType(PredType::NATIVE_INT);
  DataSet dataset =  this->createDataSet(rank, dims, type, dsname);

  // create a file dataspace
  DataSpace filespace = dataset.getSpace();

  // create a memory dataspace
  DataSpace memspace = DataSpace( rank, dims );

  // select the whole dataset as the memory dataspace
  memspace.selectAll();
  
  // select the whole dataset as the memory dataspace
  filespace.selectAll();

  // the data needs to be an array - is this memcpy ugly?
  double dat_array[nrows];
  memcpy( dat_array, &data[0], sizeof( int ) * data.size() );

  // write it
  dataset.write(dat_array, type, memspace, filespace );

};


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BookKeeper::writeData(intData3d data, string dsname){

  hsize_t nrows = data.size(); 
  hsize_t ncols = data[0].size(); 
  hsize_t nlayers = data[0][0].size(); 

  hsize_t dims[3];
  dims[0]=nrows;
  dims[1]=ncols;
  dims[2]=nlayers;
  hsize_t rank = 2;

  DataType type = DataType(PredType::NATIVE_INT);
  DataSet dataset = this->createDataSet(rank, dims, type, dsname);

  // create a file dataspace
  DataSpace filespace = dataset.getSpace();

  // create a memory dataspace
  DataSpace memspace = DataSpace( rank, dims );

  // select the whole dataset as the memory dataspace
  memspace.selectAll();
  
  // select the whole dataset as the memory dataspace
  filespace.selectAll();

  // the data needs to be an array - is this memcpy ugly?
  double dat_array[nrows];
  memcpy( dat_array, &data[0], sizeof( int ) * nrows * ncols * nlayers);

  // write it
  dataset.write(dat_array, type, memspace, filespace );
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BookKeeper::writeData(dblData1d data, string dsname){

  hsize_t nrows = data.size(); 

  hsize_t dims[2];
  dims[0]=nrows;
  dims[1]=1;
  hsize_t rank = 2;

  DataType type = DataType(PredType::NATIVE_FLOAT);
  DataSet dataset = this->createDataSet(rank, dims, type, dsname);

  // create a file dataspace
  DataSpace filespace = dataset.getSpace();

  // create a memory dataspace
  DataSpace memspace = DataSpace( rank, dims );

  // select the whole dataset as the memory dataspace
  memspace.selectAll();
  
  // select the whole dataset as the memory dataspace
  filespace.selectAll();

  // the data needs to be an array - is this memcpy ugly?
  double dat_array[nrows];
  memcpy( dat_array, &data[0], sizeof( double ) * nrows );

  // write it
  dataset.write(dat_array, type, memspace, filespace );
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BookKeeper::writeData(dblData2d data, string dsname){

  hsize_t nrows = data.size(); 
  hsize_t ncols = data[0].size(); 

  hsize_t dims[2];
  dims[0]=nrows;
  dims[1]=ncols;
  hsize_t rank = 2;

  DataType type = DataType(PredType::NATIVE_FLOAT);
  DataSet dataset = this->createDataSet(rank, dims, type, dsname);

  // create a file dataspace
  DataSpace filespace = dataset.getSpace();

  // create a memory dataspace
  DataSpace memspace = DataSpace( rank, dims );

  // select the whole dataset as the memory dataspace
  memspace.selectAll();
  
  // select the whole dataset as the memory dataspace
  filespace.selectAll();

  // the data needs to be an array - is this memcpy ugly?
  double dat_array[nrows][ncols];
  memcpy( dat_array, &data[0], sizeof( double ) * nrows * ncols);

  // write it
  dataset.write(dat_array, type, memspace, filespace );

};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BookKeeper::writeData(dblData3d data, string dsname){

  hsize_t nrows = data.size(); 
  hsize_t ncols = data[0].size(); 
  hsize_t nlayers = data[0][0].size(); 

  hsize_t dims[3];
  dims[0]=nrows;
  dims[1]=ncols;
  dims[2]=nlayers;
  hsize_t rank = 3;

  DataType type = DataType(PredType::NATIVE_FLOAT);
  DataSet dataset = this->createDataSet(rank, dims, type, dsname);

  // create a file dataspace
  DataSpace filespace = dataset.getSpace();

  // create a memory dataspace
  DataSpace memspace = DataSpace( rank, dims );

  // select the whole dataset as the memory dataspace
  memspace.selectAll();
  
  // select the whole dataset as the memory dataspace
  filespace.selectAll();

  // the data needs to be an array - is this memcpy ugly?
  double dat_array[nrows][ncols][nlayers];
  memcpy( dat_array, &data[0], sizeof( double ) * nrows * ncols * nlayers);

  // write it
  dataset.write(dat_array, type, memspace, filespace );
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BookKeeper::writeData(strData1d data, string dsname){

  hsize_t nrows = data.size(); 

  hsize_t dims[1];
  dims[0]=nrows;
  hsize_t rank = 1;

  DataType type = DataType(PredType::NATIVE_FLOAT);
  DataSet dataset = this->createDataSet(rank, dims, type, dsname);

  // create a file dataspace
  DataSpace filespace = dataset.getSpace();

  // create a memory dataspace
  DataSpace memspace = DataSpace( rank, dims );

  // select the whole dataset as the memory dataspace
  memspace.selectAll();
  
  // select the whole dataset as the memory dataspace
  filespace.selectAll();

  // the data needs to be an array - is this memcpy ugly?
  string dat_array[nrows];
  memcpy( dat_array, &data[0], sizeof( string ) * nrows);

  // write it
  dataset.write(dat_array, type, memspace, filespace );
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BookKeeper::writeData(strData2d data, string dsname){

  hsize_t nrows = data.size(); 
  hsize_t ncols = data[0].size(); 

  hsize_t dims[2];
  dims[0]=nrows;
  dims[1]=ncols;
  hsize_t rank = 2;

  DataType type = DataType(PredType::NATIVE_FLOAT);
  DataSet dataset = this->createDataSet(rank, dims, type, dsname);

  // create a file dataspace
  DataSpace filespace = dataset.getSpace();

  // create a memory dataspace
  DataSpace memspace = DataSpace( rank, dims );

  // select the whole dataset as the memory dataspace
  memspace.selectAll();
  
  // select the whole dataset as the memory dataspace
  filespace.selectAll();

  // the data needs to be an array - is this memcpy ugly?
  string dat_array[nrows];
  memcpy( dat_array, &data[0], sizeof( string ) * nrows * ncols);

  // write it
  dataset.write(dat_array, type, memspace, filespace );
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BookKeeper::writeData(strData3d data, string dsname){

  hsize_t nrows = data.size(); 
  hsize_t ncols = data[0].size(); 
  hsize_t nlayers = data[0][0].size(); 

  hsize_t dims[3];
  dims[0]=nrows;
  dims[1]=ncols;
  dims[2]=nlayers;
  hsize_t rank = 3;

  DataType type = DataType(PredType::NATIVE_FLOAT);
  DataSet dataset = this->createDataSet(rank, dims, type, dsname);

  // create a file dataspace
  DataSpace filespace = dataset.getSpace();

  // create a memory dataspace
  DataSpace memspace = DataSpace( rank, dims );

  // select the whole dataset as the memory dataspace
  memspace.selectAll();
  
  // select the whole dataset as the memory dataspace
  filespace.selectAll();

  // the data needs to be an array - is this memcpy ugly?
  string dat_array[nrows];
  memcpy( dat_array, &data[0], sizeof( string ) * nrows * ncols * nlayers);

  // write it
  dataset.write(dat_array, type, memspace, filespace );
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BookKeeper::readData(string dsname, intData1d& out_data){

  try{
    // turn off auto printing and deal with exceptions at the end
    Exception::dontPrint();
    // get the dataset open
    DataSet dataset = myDB->openDataSet(dsname);
    // get the class of the datatype used in the dataset
    H5T_class_t type_class = dataset.getTypeClass(); 
    // double check that its an integer
    if( type_class == H5T_INTEGER ) 
    {
      // oh good, it's an integer. Now figure out what kind.
      IntType inttype = dataset.getIntType();
    }
    else{
      throw GenException("The dataset " + dsname + " is not of integer type");
    }
  
    // get the file dataspace
    DataSpace filespace = dataset.getSpace();
  
    // find the rank
    int rank = filespace.getSimpleExtentNdims();
  
    // find what the dataspace dimensions are
    hsize_t dims[2];
    int ndims = filespace.getSimpleExtentDims(dims, NULL);
  
    // create a memory dataspace
    DataSpace memspace = DataSpace( rank, dims );
  
    // select everything in each dataspace
    memspace.selectAll();
    filespace.selectAll();
  
    int out_array[dims[0]];
    for (int i=0; i<dims[0]; i++){
      out_array[i]=1;
    };
  
    dataset.read( out_array, PredType::NATIVE_INT, memspace , filespace );
    
    out_data.assign(out_array, out_array + sizeof(out_array));
  } 
  catch( FileIException error )
  {
     error.printError();
  }
 
  // catch failure caused by the DataSet operations
  catch( DataSetIException error )
  {
     error.printError();
  }
 
  // catch failure caused by the DataSpace operations
  catch( DataSpaceIException error )
  {
     error.printError();
  }
 
  // catch failure caused by the DataSpace operations
  catch( DataTypeIException error )
  {
     error.printError();
  }

};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BookKeeper::readData(string dsname, intData2d& out_data){

  try{
    // turn off auto printing and deal with exceptions at the end
    Exception::dontPrint();
    // get the dataset open
    DataSet dataset = myDB->openDataSet(dsname);
    // get the class of the datatype used in the dataset
    H5T_class_t type_class = dataset.getTypeClass(); 
    // double check that its an integer
    if( type_class == H5T_INTEGER ) 
    {
      // oh good, it's an integer. Now figure out what kind.
      IntType inttype = dataset.getIntType();
    }
    else{
      throw GenException("The dataset " + dsname + " is not of integer type");
    }
  
    // get the file dataspace
    DataSpace filespace = dataset.getSpace();
  
    // find the rank
    int rank = filespace.getSimpleExtentNdims();
  
    // find what the dataspace dimensions are
    hsize_t dims[2];
    int ndims = filespace.getSimpleExtentDims(dims, NULL);
  
    // create a memory dataspace
    DataSpace memspace = DataSpace( rank, dims );
  
    // select everything in each dataspace
    memspace.selectAll();
    filespace.selectAll();
  
    int out_array[dims[0]][dims[1]];
    for (int i=0; i<dims[0]; i++){
      for (int j=0; i<dims[1]; j++){
        out_array[i][j]=1;
      };
    };
  
    dataset.read( out_array, PredType::NATIVE_INT, memspace , filespace );
    
    for (int i=0; i<dims[0]; i++){
      out_data[i].assign(out_array[i], out_array[i] + sizeof(out_array[i]));
    };
  } 
  catch( FileIException error )
  {
     error.printError();
  }
 
  // catch failure caused by the DataSet operations
  catch( DataSetIException error )
  {
     error.printError();
  }
 
  // catch failure caused by the DataSpace operations
  catch( DataSpaceIException error )
  {
     error.printError();
  }
 
  // catch failure caused by the DataSpace operations
  catch( DataTypeIException error )
  {
     error.printError();
  }

};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BookKeeper::readData(string dsname, intData3d& out_data){
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BookKeeper::readData(string dsname, dblData1d& out_data){
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BookKeeper::readData(string dsname, dblData2d& out_data){
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BookKeeper::readData(string dsname, dblData3d& out_data){
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BookKeeper::readData(string dsname, strData1d& out_data){
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BookKeeper::readData(string dsname, strData2d& out_data){
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BookKeeper::readData(string dsname, strData3d& out_data){
};
