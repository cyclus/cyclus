// MassTable class

#include "MassTable.h"

#include "CycException.h"

#include <iostream>
#include "hdf5.h"
#include "H5Cpp.h" 
#include "H5CompType.h"
#include "H5Exception.h"
#include "Env.h"

using namespace std;
using namespace H5;

MassTable* MassTable::instance_ = 0;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MassTable* MassTable::Instance() {
  // If we haven't created a MassTable yet, create it, and then and return it
  // either way.
  if (0 == instance_) {
    instance_ = new MassTable();
  }
  return instance_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MassTable::MassTable() {
  // figure out what's in the file
  initialize();
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MassTable::~MassTable() {
  //Should close the 'mass.h5' file
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Mass MassTable::getMass(Iso tope) {
  Mass toRet = nuclide_vec_[isoIndex_[tope]].mass;
  return toRet;
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
string MassTable::getName(Iso tope){ };

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void MassTable::initialize() {
  string file_path = ENV->getCyclusPath() + "/Data/mass.h5"; 

  const H5std_string filename = file_path;
  const H5std_string groupname = "ame03";
  const H5std_string datasetname = "nuclide";
  const H5std_string A_memb = "A";
  const H5std_string Z_memb = "Z";
  const H5std_string mass_memb = "mass";
  const H5std_string name_memb = "name";
  
  //check if the file is an hdf5 file first.
  if (! H5File::isHdf5(file_path)) {
    throw CycIOException("The MASS_FILE is not an hdf5 file.");
  }

  try {
    /*
     * Turn off the auto-printing when failure occurs so that we can
     * handle the errors appropriately
     */
    Exception::dontPrint();
    
    /*
     * Open the file and the dataset.
     */
    H5File* file;
    file = new H5File( filename, H5F_ACC_RDONLY );
    Group* group;
    group = new Group (file->openGroup( groupname ));
    DataSet* dataset;
    dataset = new DataSet (group->openDataSet( datasetname ));
    DataSpace* dataspace;
    dataspace = new DataSpace (dataset->getSpace( ));

    hsize_t dims_out[2];
    int ndims = dataspace->getSimpleExtentDims(dims_out, NULL);
    nuclide_len_ = dims_out[0];


    /*
     * Create a datatype for nuclide
     */
    CompType mtype( sizeof(nuclide_t) );
    mtype.insertMember( A_memb, HOFFSET(nuclide_t, A), PredType::NATIVE_INT); 
    mtype.insertMember( Z_memb, HOFFSET(nuclide_t, Z), PredType::NATIVE_INT);
    mtype.insertMember( mass_memb, HOFFSET(nuclide_t, mass), PredType::IEEE_F64LE);

    /*
     * Read two fields c and a from s1 dataset. Fields in the file
     * are found by their names "c_name" and "a_name".
     */
    nuclide_t nuclide[nuclide_len_];
    dataset->read( nuclide, mtype );

    nuclide_vec_.resize(nuclide_len_);
    copy(nuclide, nuclide + nuclide_len_, nuclide_vec_.begin() );
 
    // create a map whose indices are the Iso identifier. 
    Iso Anum;
    Iso Znum;
    Iso tope;

    for(int i = 0; i < nuclide_len_; i++) {
      Znum = nuclide[i].Z*1000;
      Anum = nuclide[i].A;
      tope = Anum+Znum;
      isoIndex_.insert(make_pair(tope, i));
    };

    /*
     * Display the fields
     */
//    cout << endl << "Field A : " << endl;
//    for(int i = 0; i < nuclide_len_; i++)
//      cout << nuclide_vec_[i].A << " ";
//    cout << endl;
//
//    cout << endl << "Field Z : " << endl;
//    for(int i = 0; i < nuclide_len_; i++)
//      cout << nuclide_vec_[i].Z << " ";
//    cout << endl;
//
//    cout << endl << "Field mass : " << endl;
//    for(int i = 0; i < nuclide_len_; i++)
//      cout << nuclide_vec_[i].mass << " ";
//    cout << endl;

    delete dataspace;
    delete group;
    delete dataset;
    delete file;
  // catch failure caused by the H5File operations
  } catch( Exception error ) {
     error.printError();
  }
};

