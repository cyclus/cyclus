// MassTable class

#include <iostream>
#include "MassTable.h"
#include "hdf5.h"
#include "H5Cpp.h" 
#include "H5Exception.h"
#define MASS_FILE "/Utility/mass.h5"

/******************************/
/*** Mass Table Class ***/
/******************************/

/***************************/
/*** Protected Functions ***/
/***************************/
using namespace std;
using namespace H5;

void MassTable::initialize()
{
  const H5std_string filename = MASS_FILE;
  const H5std_string groupname = "ame03";
  const H5std_string datasetname = "nuclide";
  const H5std_string dataname = "mass";

  try{
    /*
     * Turn off the auto-printing when failure occurs so that we can
     * handle the errors appropriately
     */
    Exception::dontPrint();
  
    /*
     * Open the specified file
     */
    H5File file( filename, H5F_ACC_RDONLY );
    // Open the group within the file
    Group group = file.openGroup( groupname );
    // get the dataset within the group
    DataSet dataset = group.openDataSet( datasetname );
    // get the type of the dataset
    H5T_class_t type_class = dataset.getTypeClass();
    // get the dataspace within the dataset
    DataSpace dataspace = dataset.getSpace();
    // get the number of dimensions in the dataspace
    int rank = dataspace.getSimpleExtentNdims();
    /*
     * Get the dimension size of each dimension in the dataspace
     */
    hsize_t dims_out[2];
    int ndims = dataspace.getSimpleExtentDims(dims_out, NULL);

    // display them.
    cout << "rank " << rank << ", dimensions " <<
            (unsigned long)(dims_out[0]) << " x " <<
            (unsigned long)(dims_out[1]) << endl;
  }

  // catch failure caused by the H5File operations
  catch( FileIException error )
  {
     error.printError();
  }
 
  // catch failure caused by the Group operations
  catch( GroupIException error )
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
 
  // catch failure caused by the DataType operations
  catch( DataTypeIException error )
  {
     error.printError();
  }
 

  /***********************************************************************************************
	hid_t  mass_file_id, ame_grp_id, nuc_dset_id,
         mass_dset_id, mass_dspc_id, nuc_data_id;
	herr_t nuc_status;

	mass_file_id = H5::H5File::openFile(MASS_FILE, H5F_ACC_RDONLY, H5P_DEFAULT);	//Opens the hdf5 file
	ame_grp_id   = H5::GroupOpen(mass_file_id, "/ame03");	 	  //Opens the Group
  nuc_dset_id  = H5Dopen(ame_grp_id, "/nuclide"); 		//Opens the Compound nuclide Dataset
  mass_dset_id = H5Dopen(nuc_dset_id, "/mass");	  	//Opens the mass Dataset
	mass_dspc_id  = H5Dget_space(mass_dset_id);				                  //Gets the filespace in order to...
  nuc_data_len = H5Sget_simple_extent_npoints(mass_dspc_id);		      //Calculate the number of data entries.

  nuc_data_id = H5Tcreate(H5T_COMPOUND, sizeof(Nuclide) );	//Makes an id indicating the data structure.
	nuc_status  = H5Tinsert(nuc_data_id, "A",     HOFFSET(Nuclide, A),    H5T_STD_I8LE );// unsigned int
	nuc_status  = H5Tinsert(nuc_data_id, "Z",    HOFFSET(Nuclide, Z),     H5T_STD_I8LE); // unsigned int
	nuc_status  = H5Tinsert(nuc_data_id, "mass",  HOFFSET(Nuclide, mass), H5T_IEEE_F64LE); // 64 bit float
	nuc_status  = H5Tinsert(nuc_data_id, "name",  HOFFSET(Nuclide, name), H5T_C_S1); // 1 byte string

	//Initializes an array of the data struct and fills it!
	nuc_data   = new Nuclide[nuc_data_len];
	nuc_status = H5Dread(nuc_dset_id, nuc_data_id, H5S_ALL, H5S_ALL, H5P_DEFAULT, nuc_data);

	//Closes the hdf5 file.
	nuc_status = H5Dclose(nuc_dset_id);
	nuc_status = H5Dclose(ame_grp_id);
	nuc_status = H5Fclose(mass_file_id);
  */
};

/****************************/
/*** MassTable Constructors ***/
/****************************/

MassTable::MassTable()
{
	//Empty storage component
	initialize();
};

MassTable::~MassTable()
{
	//Should close the 'mass.h5' file
};

