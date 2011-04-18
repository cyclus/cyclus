// BookKeeper.cpp
// Implements the BookKeeper class
#include "BookKeeper.h"
#include "Timer.h"
#include "boost/multi_array.hpp"

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
    DataSpace* dataspace = new DataSpace(rank, dims);

    // create a dataset to match the dataspace
    dataset = this->getDB()->createDataSet(dsName, type, *dataspace) ; 

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
void BookKeeper::prepareSpaces(string dsname, DataType type, DataSpace &memspace, 
    DataSpace &filespace, DataSet &dataset){
    
    // define the dataset to match the dataspace
    dataset = this->getDB()->createDataSet(dsname, type, memspace) ; 

    // define the file dataspace to match the dataset
    filespace = dataset.getSpace();
  
    // select the whole dataset as the memory dataspace
    memspace.selectAll();
    
    // select the whole dataset as the memory dataspace
    filespace.selectAll();

};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BookKeeper::writeData(intData1d data, string dsname){ 
  try {
    hsize_t nrows = data.size(); 
  
    hsize_t dims[1];
    dims[0]=nrows;
    hsize_t rank = 1;

    // create a memory dataspace
    DataSpace memspace = DataSpace( rank, dims);
    // create a file dataspace 
    DataSpace filespace;
    // create a dataset
    DataSet dataset;
    // name the type
    DataType type = DataType(PredType::NATIVE_INT);
    // prepare the spaces
    this->prepareSpaces(dsname, type, memspace, filespace, dataset);
    
    // the data needs to be an array - is this memcpy ugly?
    double dat_array[nrows];
    memcpy( dat_array, &data[0], sizeof( int ) * data.size() );
  
    // write it
    dataset.write(dat_array, type, memspace, filespace );

  }
  catch( FileIException error )
  {
    error.printError();
  }
  catch( DataSetIException error )
  {
    error.printError();
  }
  catch( DataSpaceIException error )
  {
    error.printError();
  }
  catch( GroupIException error )
  {
    error.printError();
  }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BookKeeper::writeData(intData2d data, string dsname){
  try {
    hsize_t nrows = data.size(); 
    hsize_t ncols = data[0].size(); 
    
    hsize_t dims[] = {nrows, ncols};
    hsize_t rank = 2;
    
    // create a memory dataspace
    DataSpace memspace = DataSpace( rank, dims );
    // create a file dataspace 
    DataSpace filespace;
    // create a dataset
    DataSet dataset;
    // name the type
    DataType type = DataType(PredType::NATIVE_INT);
    // prepare the spaces
    this->prepareSpaces(dsname, type, memspace, filespace, dataset);
    
    // the data needs to be an array - is this memcpy ugly?
    int dat_array[nrows][ncols];
    for(int row=0; row<nrows; row++){
      memcpy( dat_array[row], &data[row][0], sizeof( int ) * ncols );
    };

    // write it
    dataset.write(dat_array, type, memspace, filespace );
  }
  catch( FileIException error )
  {
    error.printError();
  }
  catch( DataSpaceIException error )
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
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BookKeeper::writeData(intData3d data, string dsname){

  try {
    hsize_t nrows = data.size(); 
    hsize_t ncols = data[0].size(); 
    hsize_t nlayers = data[0][0].size(); 
    
    hsize_t dims[] = {nrows, ncols,nlayers};
    hsize_t rank = 3;
    
    // create a memory dataspace
    DataSpace memspace = DataSpace( rank, dims );
    // create a file dataspace 
    DataSpace filespace;
    // create a dataset
    DataSet dataset;
    // name the type
    DataType type = DataType(PredType::NATIVE_INT);
    // prepare the spaces
    this->prepareSpaces(dsname, type, memspace, filespace, dataset);
    
    // the data needs to be an array - is this memcpy ugly?
    int dat_array[nrows][ncols][nlayers];
    for(int row=0; row<nrows; row++){
      for(int col=0; col<ncols; col++){
        memcpy( dat_array[row][col], &data[row][col][0], sizeof( int ) * nlayers);
      };
    };

    // write it
    dataset.write(dat_array, type, memspace, filespace );
  }
  catch( FileIException error )
  {
    error.printError();
  }
  catch( DataSpaceIException error )
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
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BookKeeper::writeData(dblData1d data, string dsname){
  try {
    hsize_t nrows = data.size(); 
  
    hsize_t dims[1];
    dims[0]=nrows;
    hsize_t rank = 1;

    // create a memory dataspace
    DataSpace memspace = DataSpace( rank, dims );
    // create a file dataspace 
    DataSpace filespace;
    // create a dataset
    DataSet dataset;
    // name the type
    DataType type = DataType(PredType::NATIVE_FLOAT);
    // prepare the spaces
    this->prepareSpaces(dsname, type, memspace, filespace, dataset);
  
    // the data needs to be an array - is this memcpy ugly?
    double dat_array[nrows];
    memcpy( dat_array, &data[0], sizeof( double ) * data.size() );
  
    // write it
    dataset.write(dat_array, type, memspace, filespace );
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
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BookKeeper::writeData(dblData2d data, string dsname){
  try {
    hsize_t nrows = data.size(); 
    hsize_t ncols = data[0].size(); 
    
    hsize_t dims[] = {nrows, ncols};
    hsize_t rank = 2;

    // create a memory dataspace
    DataSpace memspace = DataSpace( rank, dims );
    // create a file dataspace 
    DataSpace filespace;
    // create a dataset
    DataSet dataset;
    // name the type
    DataType type = DataType(PredType::NATIVE_FLOAT);
    // prepare the spaces
    this->prepareSpaces(dsname, type, memspace, filespace, dataset);

    // the data needs to be an array - is this memcpy ugly?
    double dat_array[nrows][ncols];
    for(int row=0; row<nrows; row++){
      memcpy( dat_array[row], &data[row][0], sizeof( double ) * ncols );
    };

    // write it
    dataset.write(dat_array, type, memspace, filespace );
  }
  catch( FileIException error )
  {
    error.printError();
  }
  catch( DataSpaceIException error )
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
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BookKeeper::writeData(dblData3d data, string dsname){
  try {
    hsize_t nrows = data.size(); 
    hsize_t ncols = data[0].size(); 
    hsize_t nlayers = data[0][0].size(); 
    
    hsize_t dims[] = {nrows, ncols,nlayers};
    hsize_t rank = 3;

    // create a memory dataspace
    DataSpace memspace = DataSpace( rank, dims );
    // create a file dataspace 
    DataSpace filespace;
    // create a dataset
    DataSet dataset;
    // name the type
    DataType type = DataType(PredType::NATIVE_FLOAT);
    // prepare the spaces
    this->prepareSpaces(dsname, type, memspace, filespace, dataset);
    
    // the data needs to be an array - is this memcpy ugly?
    double dat_array[nrows][ncols][nlayers];
    for(int row=0; row<nrows; row++){
      for(int col=0; col<ncols; col++){
        memcpy( dat_array[row][col], &data[row][col][0], sizeof( double ) * nlayers);
      };
    };

    // write it
    dataset.write(dat_array, type, memspace, filespace );
  }
  catch( FileIException error )
  {
    error.printError();
  }
  catch( DataSpaceIException error )
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
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BookKeeper::writeData(strData1d data, string dsname){
  try {
    hsize_t nrows = data.size(); 
  
    hsize_t dims[1];
    dims[0]=nrows;
    hsize_t rank = 1;
  
    // create a memory dataspace
    DataSpace memspace = DataSpace( rank, dims );
    // create a file dataspace 
    DataSpace filespace;
    // create a dataset
    DataSet dataset;
    //create a variable length string types
    StrType vls_type(0, H5T_VARIABLE); 
    DataType type = DataType(vls_type);
    // prepare the spaces
    this->prepareSpaces(dsname, type, memspace, filespace, dataset);

    // the data needs to be an array
    string dat_array[nrows];
    dat_array;
    for(int row=0; row < nrows;row++){
      dat_array[row]=data[row];
    };
  
    // write it
    dataset.write(dat_array, type, memspace, filespace );
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
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BookKeeper::writeData(strData2d data, string dsname){
  try {
    hsize_t nrows = data.size(); 
    hsize_t ncols = data[0].size(); 
    
    hsize_t dims[] = {nrows, ncols};
    hsize_t rank = 2;

    // create a memory dataspace
    DataSpace memspace = DataSpace( rank, dims );
    // create a file dataspace 
    DataSpace filespace;
    // create a dataset
    DataSet dataset;
    //create a variable length string types
    StrType vls_type(0, H5T_VARIABLE); 
    DataType type = DataType(vls_type);
    // prepare the spaces
    this->prepareSpaces(dsname, type, memspace, filespace, dataset);
    
    // the data needs to be an array
    string dat_array[nrows][ncols];
    for(int row=0; row<nrows; row++){
      for(int col=0; col<ncols; col++){
        dat_array[row][col]=data[row][col];
      };
    };

    // write it
    dataset.write(dat_array, type, memspace, filespace );

  }
  catch( FileIException error )
  {
    error.printError();
  }
  catch( DataSpaceIException error )
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
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void BookKeeper::writeData(strData3d data, string dsname){
  try {
    hsize_t nrows = data.size(); 
    hsize_t ncols = data[0].size(); 
    hsize_t nlayers = data[0][0].size(); 
    
    hsize_t dims[] = {nrows, ncols,nlayers};
    hsize_t rank = 3;

    // create a memory dataspace
    DataSpace memspace = DataSpace( rank, dims );
    // create a file dataspace 
    DataSpace filespace;
    // create a dataset
    DataSet dataset;
    //create a variable length string types
    StrType vls_type(0, H5T_VARIABLE); 
    DataType type = DataType(vls_type);
    // prepare the spaces
    this->prepareSpaces(dsname, type, memspace, filespace, dataset);
    
    // the data needs to be an array 
    string dat_array[nrows][ncols][nlayers];
    for(int row=0; row<nrows; row++){
      for(int col=0; col<ncols; col++){
        for(int layer=0; layer<nlayers;layer++){
          dat_array[row][col][layer]=data[row][col][layer];
        };
      };
    };

    // write it
    dataset.write(dat_array, type, memspace, filespace );
  }
  catch( FileIException error )
  {
    error.printError();
  }
  catch( DataSpaceIException error )
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
    IntType inttype;
    if( type_class == H5T_INTEGER ) 
    {
      // oh good, it's an integer. Now figure out what kind.
      inttype = dataset.getIntType();
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
  
    dataset.read( out_array, inttype, memspace , filespace );
    
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
    IntType inttype;
    if( type_class == H5T_INTEGER ) 
    {
      // oh good, it's an integer. Now figure out what kind.
      inttype = dataset.getIntType();
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
      for (int j=0; j<dims[1]; j++){
        out_array[i][j]=1;
      };
    };
  
    dataset.read( out_array, inttype, memspace , filespace );

    for(int row=0; row<dims[0]; row++){
      for(int col=0; col<dims[1]; col++){
        out_data[row][col] = out_array[row][col];
      };
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

  try{
    // turn off auto printing and deal with exceptions at the end
    Exception::dontPrint();
    // get the dataset open
    DataSet dataset = myDB->openDataSet(dsname);
    // get the class of the datatype used in the dataset
    H5T_class_t type_class = dataset.getTypeClass(); 
    // double check that its an integer
    IntType inttype;
    if( type_class == H5T_INTEGER ) 
    {
      // oh good, it's an integer. Now figure out what kind.
      inttype = dataset.getIntType();
    }
    else{
      throw GenException("The dataset " + dsname + " is not of integer type");
    }
  
    // get the file dataspace
    DataSpace filespace = dataset.getSpace();
  
    // find the rank
    int rank = filespace.getSimpleExtentNdims();
  
    // find what the dataspace dimensions are
    hsize_t dims[3];
    int ndims = filespace.getSimpleExtentDims(dims, NULL);
  
    // create a memory dataspace
    DataSpace memspace = DataSpace( rank, dims );
  
    // select everything in each dataspace
    memspace.selectAll();
    filespace.selectAll();
  
    int out_array[dims[0]][dims[1]][dims[2]];
    for (int i=0; i<dims[0]; i++){
      for (int j=0; j<dims[1]; j++){
        for (int k=0; k<dims[2]; k++){
          out_array[i][j][k]=1;
        };
      };
    };
  
    dataset.read( out_array, inttype, memspace , filespace );

    for(int row=0; row<dims[0]; row++){
      for(int col=0; col<dims[1]; col++){
        for(int layer=0; layer<dims[2];layer++){
          out_data[row][col][layer] = out_array[row][col][layer];
        };
      };
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
void BookKeeper::readData(string dsname, dblData1d& out_data){
  try{
    // turn off auto printing and deal with exceptions at the end
    Exception::dontPrint();
    // get the dataset open
    DataSet dataset = myDB->openDataSet(dsname);
    // get the class of the datatype used in the dataset
    H5T_class_t type_class = dataset.getTypeClass(); 

    // double check that its a double
    FloatType dbltype;
    if( type_class == H5T_FLOAT ) 
    {
      // oh good, it's a double. Now figure out what kind.
      dbltype = dataset.getFloatType();
    }
    else{
      throw GenException("The dataset " + dsname + " is not of float type");
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
  
    double out_array[dims[0]];
    for (int i=0; i<dims[0]; i++){
      out_array[i]=1;
    };
  
    dataset.read( out_array, dbltype, memspace , filespace );

    for(int row=0; row<dims[0]; row++){
      out_data[row]= out_array[row];
    };
    
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
void BookKeeper::readData(string dsname, dblData2d& out_data){
  try{
    // turn off auto printing and deal with exceptions at the end
    Exception::dontPrint();
    // get the dataset open
    DataSet dataset = myDB->openDataSet(dsname);
    // get the class of the datatype used in the dataset
    H5T_class_t type_class = dataset.getTypeClass(); 
    // double check that its a double
    FloatType dbltype;
    if( type_class == H5T_FLOAT ) 
    {
      // oh good, it's a double. Now figure out what kind.
      dbltype = dataset.getFloatType();
    }
    else{
      throw GenException("The dataset " + dsname + " is not of float type");
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
  
    double out_array[dims[0]][dims[1]];
    for (int i=0; i<dims[0]; i++){
      for (int j=0; j<dims[1]; j++){
        out_array[i][j]=1.0;
      };
    };
  
    dataset.read( out_array, dbltype, memspace , filespace );

    for(int row=0; row<dims[0]; row++){
      for(int col=0; col<dims[1]; col++){
        out_data[row][col] = out_array[row][col];
      };
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
void BookKeeper::readData(string dsname, dblData3d& out_data){

  try{
    // turn off auto printing and deal with exceptions at the end
    Exception::dontPrint();
    // get the dataset open
    DataSet dataset = myDB->openDataSet(dsname);
    // get the class of the datatype used in the dataset
    H5T_class_t type_class = dataset.getTypeClass(); 
    // double check that its a double
    FloatType dbltype;
    if( type_class == H5T_FLOAT ) 
    {
      // oh good, it's a double. Now figure out what kind.
      dbltype = dataset.getFloatType();
    }
    else{
      throw GenException("The dataset " + dsname + " is not of double type");
    }
  
    // get the file dataspace
    DataSpace filespace = dataset.getSpace();
  
    // find the rank
    int rank = filespace.getSimpleExtentNdims();
  
    // find what the dataspace dimensions are
    hsize_t dims[3];
    int ndims = filespace.getSimpleExtentDims(dims, NULL);
  
    // create a memory dataspace
    DataSpace memspace = DataSpace( rank, dims );
  
    // select everything in each dataspace
    memspace.selectAll();
    filespace.selectAll();
  
    double out_array[dims[0]][dims[1]][dims[2]];
    for (int i=0; i<dims[0]; i++){
      for (int j=0; j<dims[1]; j++){
        for (int k=0; k<dims[2]; k++){
          out_array[i][j][k]=1.0;
        };
      };
    };
  
    dataset.read( out_array, dbltype, memspace , filespace );

    for(int row=0; row<dims[0]; row++){
      for(int col=0; col<dims[1]; col++){
        for(int layer=0; layer<dims[2];layer++){
          out_data[row][col][layer] = out_array[row][col][layer];
        };
      };
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
void BookKeeper::readData(string dsname, strData1d& out_data){
  try{
    // turn off auto printing and deal with exceptions at the end
    Exception::dontPrint();
    // get the dataset open
    DataSet dataset = myDB->openDataSet(dsname);
    // get the class of the datatype used in the dataset
    H5T_class_t type_class = dataset.getTypeClass(); 

    // double check that its a string
    StrType strtype;
    if( type_class == H5T_STRING ) 
    {
      // oh good, it's a string. Now figure out what kind.
      strtype = dataset.getStrType();
    }
    else{
      throw GenException("The dataset " + dsname + " is not of string type");
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
  
    string out_array[dims[0]];
    for (int i=0; i<dims[0]; i++){
      out_array[i]="";
    };
  
    dataset.read( out_array, strtype, memspace , filespace );

    for(int row=0; row<dims[0]; row++){
      out_data[row] = out_array[row];
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
void BookKeeper::readData(string dsname, strData2d& out_data){
  try{
    // turn off auto printing and deal with exceptions at the end
    Exception::dontPrint();
    // get the dataset open
    DataSet dataset = myDB->openDataSet(dsname);
    // get the class of the datatype used in the dataset
    H5T_class_t type_class = dataset.getTypeClass(); 
    // double check that its a string
    StrType strtype;
    if( type_class == H5T_STRING ) 
    {
      // oh good, it's a string. Now figure out what kind.
      strtype = dataset.getStrType();
    }
    else{
      throw GenException("The dataset " + dsname + " is not of string type");
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
  
    string out_array[dims[0]][dims[1]];
    for (int i=0; i<dims[0]; i++){
      for (int j=0; j<dims[1]; j++){
        out_array[i][j]="";
      };
    };
  
    dataset.read( out_array, strtype, memspace , filespace );

    for(int row=0; row<dims[0]; row++){
      for(int col=0; col<dims[1]; col++){
        out_data[row][col] = out_array[row][col];
      };
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
void BookKeeper::readData(string dsname, strData3d& out_data){

  try{
    // turn off auto printing and deal with exceptions at the end
    Exception::dontPrint();
    // get the dataset open
    DataSet dataset = myDB->openDataSet(dsname);
    // get the class of the datatype used in the dataset
    H5T_class_t type_class = dataset.getTypeClass(); 
    // double check that its an string
    StrType  strtype;
    if( type_class == H5T_STRING ) 
    {
      // oh good, it's a string. Now figure out what kind.
      strtype = dataset.getStrType();
    }
    else{
      throw GenException("The dataset " + dsname + " is not of string type");
    }
  
    // get the file dataspace
    DataSpace filespace = dataset.getSpace();
  
    // find the rank
    int rank = filespace.getSimpleExtentNdims();
  
    // find what the dataspace dimensions are
    hsize_t dims[3];
    int ndims = filespace.getSimpleExtentDims(dims, NULL);
  
    // create a memory dataspace
    DataSpace memspace = DataSpace( rank, dims );
  
    // select everything in each dataspace
    memspace.selectAll();
    filespace.selectAll();
  
    string out_array[dims[0]][dims[1]][dims[2]];
    for (int i=0; i<dims[0]; i++){
      for (int j=0; j<dims[1]; j++){
        for (int k=0; k<dims[2]; k++){
          out_array[i][j][k]="";
        };
      };
    };
  
    dataset.read( out_array, strtype, memspace , filespace );

    for(int row=0; row<dims[0]; row++){
      for(int col=0; col<dims[1]; col++){
        for(int layer=0; layer<dims[2];layer++){
          out_data[row][col][layer] = out_array[row][col][layer];
        };
      };
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
