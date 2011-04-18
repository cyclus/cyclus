// BookKeeper.h
#if !defined(_BOOKKEEPER)
#define _BOOKKEEPER
#include <string>
#include <map>
#include <vector>
#include "boost/multi_array.hpp"

#include "H5Cpp.h"
#include "hdf5.h"
#define BI BookKeeper::Instance()

using namespace std;
using namespace H5;


//typedef vector< int > intData1d ; 
//typedef vector< double > dblData1d ; 
//typedef vector< string > strData1d ; 
//typedef vector< vector < int > > intData2d ; 
//typedef vector< vector < double > > dblData2d ; 
//typedef vector< vector < string > > strData2d ; 
//typedef vector< vector < vector < int > > > intData3d ; 
//typedef vector< vector < vector < double > > > dblData3d ; 
//typedef vector< vector < vector < string > > > strData3d ; 
typedef boost::multi_array<int, 1> intData1d;
typedef intData1d::index int1didx;
typedef boost::multi_array<int, 2> intData2d;
typedef intData2d::index int2didx;
typedef boost::multi_array<int, 3> intData3d;
typedef intData3d::index int3didx;
typedef boost::multi_array<double, 1> dblData1d;
typedef dblData1d::index dbl1didx;
typedef boost::multi_array<double, 2> dblData2d;
typedef dblData2d::index dbl2didx;
typedef boost::multi_array<double, 3> dblData3d;
typedef dblData3d::index dbl3didx;
typedef boost::multi_array<string, 1> strData1d;
typedef strData1d::index str1didx;
typedef boost::multi_array<string, 2> strData2d;
typedef strData2d::index str2didx;
typedef boost::multi_array<string, 3> strData3d;
typedef strData3d::index str3didx;


/**
 * A (singleton) class for handling I/O.
 */
class BookKeeper {
private:
  /**
   * A pointer to this BookKeeper once it has been initialized.
	 */
  static BookKeeper* _instance;
		
  /**
   * The HDF5 output database for the simulation this BookKeeper is 
   * responsible for.
	 */
  H5File* myDB;
		
  /**
   * Stores the final filename we'll use for the DB, since we use it 
   * in multiple places and don't want there to be any ambiguity.
	 */
  string dbName;

  /**
   * True iff the db is open.
   */
  bool dbIsOpen;

protected:
  /**
   * The (protected) constructor for this class, which can only be 
   * called indirectly by the client.
	 */
  BookKeeper();

  /**
   * Creates a group with a title.
   */
  Group* newGroup(string title);

  /**
   * Create a property list for a dataset and set up fill values.
	 */
  DSetCreatPropList* dsPropList(){};

  /**
   * This creates a 2d dataset out of a dataspace and some data
   *
   * @param ds is a pointer to a dataspace object of the right 
   * dimesions
   * @param data is a pointer (?) to the data to put into the set 
   *
   * @return a pointer to an H5/CPP DataSet object in memory
	 */

public:
		
  /**
   * Gives all simulation objects global access to the BookKeeper by 
   * returning a pointer to it.
   * Like the Highlander, there can be only one.
	 *
   * @return a pointer to the BookKeeper
	 */
  static BookKeeper* Instance();
		
  /**
   * Creates a database file with the default name, cyclus.h5. 
	 */
  void createDB();

  /**
   * Creates a database file with the name indicated. 
	 *
   * @param name is the name of the hdf5 database file. Should end in .h5
	 */
  void createDB(string name);

  /**
   * Creates a dataset with the name,type, and dimesions indicated 
	 *
   * @param rank is the rank of the dataset
   * @param dims are the dimensions of the dataset 
   * @param type is type of data to be placed in the dataspace
   * @param name is the name of the dataset 
	 */
  DataSet createDataSet(hsize_t rank, hsize_t* dims, DataType type, string name);

  /**
   * Returns a handle to the database this BookKeeper is maintaining.
	 *
   * @return the handle to the database
	 */
  H5File* getDB();

  /**
   * Opens the output database in memory space.
	 */
  void openDB();

  /**
   * Closes the database this BookKeeper is maintaining.
	 */
  void closeDB();
  
  /**
   * Returns whether it's open
   */
  bool isOpen(){return dbIsOpen;};

  /**
   * Returns whether the group exists in the database
   *
   * @param grp the name of the group being queried
   */
  bool isGroup(string grp);

  /**
   * Returns the name of the database
   */
  string getDBName(){return dbName;};

  /**
   * Prepares the file and memory dataspaces for data to be written
   *
   *
   */
  void prepareSpaces(string dsname, DataType type, DataSpace &memspace, 
      DataSpace &filespace, DataSet &dataset);

  /**
   * Puts a one dimensional vector of integer data in the indicated 
   * dataspace
   *
   * @param data is the data
   * @param dspace is the dataspace
   */
  void writeData(intData1d data, string dsname);

  /**
   * Puts a one dimensional vector of integer data in the indicated 
   * dataspace
   *
   * @param data is the data
   * @param dspace is the dataspace
   */
  void writeData(intData2d data, string dsname);

  /**
   * Puts a one dimensional vector of integer data in the indicated 
   * dataspace
   *
   * @param data is the data
   * @param dspace is the dataspace
   */
  void writeData(intData3d data, string dsname);

  /**
   * Puts a one dimensional vector of double data in the indicated 
   * dataspace
   *
   * @param data is the data
   * @param dspace is the dataspace
   */
  void writeData(dblData1d data, string dsname);

  /**
   * Puts a two dimensional vector of double data in the indicated 
   * dataspace
   *
   * @param data is the data
   * @param dspace is the dataspace
   */
  void writeData(dblData2d data, string dsname);

  /**
   * Puts a three dimensional vector of double data in the indicated 
   * dataspace
   *
   * @param data is the data
   * @param dspace is the dataspace
   */
  void writeData(dblData3d data, string dsname);

  /**
   * Puts a one dimensional vector of string data in the indicated 
   * dataspace
   *
   * @param data is the data
   * @param dspace is the dataspace
   */
  void writeData(strData1d data, string dsname);

  /**
   * Puts a two dimensional vector of string data in the indicated 
   * dataspace
   *
   * @param data is the data
   * @param dspace is the dataspace
   */
  void writeData(strData2d data, string dsname);

  /**
   * Puts a three dimensional vector of string data in the indicated 
   * dataspace
   *
   * @param data is the data
   * @param dspace is the dataspace
   */
  void writeData(strData3d data, string dsname);

  /**
   * Reads 1d integer data from the dataset indicated.
   *
   * @param name is the hyperspace to read out
   * @param out_data is the vector to write to
   */
  void readData(string name, intData1d& out_data);

  /**
   * Reads 2d integer data from the dataset indicated.
   *
   * @param name is the name of the dataset
   * @param out_data is the vector to write to
   */
  void readData(string name, intData2d& out_data);

  /**
   * Reads 3d integer data from the dataset indicated.
   *
   * @param name is the name of the dataset
   * @param out_data is the vector to write to
   */
   void readData(string name, intData3d& out_data);
  /**
   * Reads data from the dataset indicated.
   *
   * @param name is the hyperspace to read out
   * @param out_data is the vector to write to
   */
  void readData(string name, dblData1d& out_data);

  /**
   * Reads data from the dataset indicated.
   *
   * @param name is the name of the dataset
   * @param out_data is the vector to write to
   */
  void readData(string name, dblData2d& out_data);

  /**
   * Reads data from the dataset indicated.
   *
   * @param name is the name of the dataset
   * @param out_data is the vector to write to
   */
   void readData(string name, dblData3d& out_data);

  /**
   * Reads data from the dataset indicated.
   *
   * @param name is the name of the dataset
   * @param out_data is the vector to write to
   */
  void readData(string name, strData1d& out_data);

  /**
   * Reads data from the dataset indicated.
   *
   * @param name is the name of the dataset
   * @param out_data is the vector to write to
   */
  void readData(string name, strData2d& out_data);

  /**
   * Reads data from the dataset indicated.
   *
   * @param name is the name of the dataset
   * @param out_data is the vector to write to
   */
   void readData(string name, strData3d& out_data);


};
#endif
