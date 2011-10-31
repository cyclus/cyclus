// BookKeeper.h
#if !defined(_BOOKKEEPER)
#define _BOOKKEEPER

#include <string>
#include <vector>
#include "boost/multi_array.hpp"
#include "H5Cpp.h"
#include "hdf5.h"

#include "Model.h"
#include "Material.h"
#include "Message.h"

#define BI BookKeeper::Instance()

using namespace H5;

// homogeneous boost multidimensional arrays 
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
typedef boost::multi_array<std::string, 1> strData1d;
typedef strData1d::index str1didx;
typedef boost::multi_array<std::string, 2> strData2d;
typedef strData2d::index str2didx;
typedef boost::multi_array<std::string, 3> strData3d;
typedef strData3d::index str3didx;

/**
 * A (singleton) class for handling I/O.
 */
class BookKeeper {
private:
  /**
   * A pointer to this BookKeeper once it has been initialized.
     */
  static BookKeeper* instance_;
        
  /**
   * The HDF5 output database for the simulation this BookKeeper is 
   * responsible for.
     */
  H5File* myDB_;
        
  /**
   * Stores the final filename we'll use for the DB, since we use it 
   * in multiple places and don't want there to be any ambiguity.
     */
  std::string dbName_;

  /**
   * True iff the db is open.
   */
  bool dbIsOpen_;

  /**
   * True iff the db is open.
   */
  bool dbExists_;

protected:
  /**
   * The (protected) constructor for this class, which can only be 
   * called indirectly by the client.
     */
  BookKeeper();
  
  // facility or institution model struct
  typedef struct model_t{
    int ID;                 /**< An integer indicating the model ID >**/
    char name[64];            /**< A std::string indicating the name of the template >**/ 
    char modelImpl[64];       /**< A std::string indicating the model implementation >**/
    int parentID; /**< An integer of the model's parent's ID >**/
  } model_t;

  // facility, market, converter, or institution model struct
  typedef struct trans_t{
    int supplierID;         /**< An integer indicating the supplier model ID >**/
    int requesterID;        /**< An integer indicating the requester model ID >**/
    int materialID;         /**< An integer indicating the material object ID >**/
    int timestamp;          /**< An integer indicating the month >**/
    double price;           /**< A double indicating the transaction price >**/   
    char commodName[64];   /**< the name of the commodity >**/
  } trans_t;

  // material history struct
  typedef struct mat_hist_t{
    int materialID;         /**< An integer indicating the material object ID >**/
    int timestamp;          /**< An integer indicating the month >**/
    int iso;                /**< An integer indicating the nuclide ID >**/   
    double comp;            /**< The kg or moles of the iso in the material at that time >**/
  } mat_hist_t;

  // volume history struct
  typedef struct vol_hist_t{
    int volID;              /**< An integer indicating the volume object ID >**/
    int timestamp;          /**< An integer indicating the month >**/
  } vol_hist_t;

  /**
   * Stores the transactions that have taken place during the simulation.
   */
  std::vector<trans_t> transactions_;

  /**
   * Stores the material changes that have taken place during the simulation.
   */
  std::vector<mat_hist_t> materials_;

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
  void createDB(std::string name);

  /* function not currently used (MJG) */
  /* /\** */
  /*  * Creates a dataset with the name,type, and dimesions indicated  */
  /*    * */
  /*  * @param rank is the rank of the dataset */
  /*  * @param dims are the dimensions of the dataset  */
  /*  * @param type is type of data to be placed in the dataspace */
  /*  * @param name is the name of the dataset  */
  /*    *\/ */
  /* DataSet createDataSet(hsize_t rank, hsize_t* dims, DataType type, std::string name); */

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
  bool isOpen(){return dbIsOpen_;};
  
  /**
   * Returns whether it exists
   */
  bool exists(){return dbExists_;};

  /* Function only used in tests (MJG) */
  /**
   * Returns whether the group exists in the database
   *
   * @deprecated not used at all. Needs removal (rcarlsen).
   *
   * @param grp the name of the group being queried
   */
  bool isGroup(std::string grp);

  /**
   * Returns the name of the database
   */
  std::string getDBName(){return dbName_;};

  /**
   * Register the transaction in the BookKeeper's map of transactions
   *
   * @param msg the message containing the transaction
   * @param manifest a vector the materials fulfilling this transaction
   */
  void registerTrans(Message* msg, std::vector<Material*> manifest);

  /**
   * Register the materialin the BookKeeper's map of material changes
   *
   * @param mat the material with a history
   */
  void registerMatChange(Material* mat);

  /**
   * Write a list of the facility/inst/market models in the simulation
   *
   * @param type the model type (i.e. insts, facilities, or markets)
   */
  void writeModelList(ModelType type);

  /* Work in Progress (MJG) */
  /* /\** */
  /*  * Set up parameters to write the model list to a .h5 file */
  /*  * @param type the model type (as above) */
  /*  * @param ID_memb id hdf5 string */
  /*  * @param name_memb name hdf5 string */
  /*  * @param modelImpl_memb implementation hdf5 string */
  /*  * @param output_name output hdf5 string */
  /*  * @param subgroup_name subgroup name std::string */
  /*  * @param dataset_name dataset name std::string */
  /*  * @param numStructs number of models to write (if numModels = 0; numStructs = 1) */
  /*  * @param numModels number of models to write */
  /*  * we use numStrucs and numModels so that HDF5 behaves well when numModels = 0 */
  /*  * @param modelList the list of models to write (created by setUpModelList) */
  /*  *\/ */
  /* void setUpModelWrite(ModelType type,       \ */
  /*            H5std_string ID_memb, H5std_string name_memb,  \ */
  /*            H5std_string modelImpl_memb, H5std_string output_name, \ */
  /*            std::string subgroup_name, std::string dataset_name, \ */
  /*            int numStructs, int numModels, model_t* modelList); */

  /**
   * Given information from the set-up, write the model list
   * @param ID_memb id hdf5 string
   * @param name_memb name hdf5 string
   * @param modelImpl_memb implementation hdf5 string
   * @param output_name output hdf5 string
   * @param subgroup_name subgroup name std::string
   * @param dataset_name dataset name std::string
   * @param numStructs number of models to write (if numModels = 0; numStructs = 1)
   * @param numModels number of models to write
   * we use numStrucs and numModels so that HDF5 behaves well when numModels = 0
   * @param modelList the list of models to write (created by setUpModelList)
   */
  void doModelWrite(H5std_string ID_memb, H5std_string name_memb,  \
        H5std_string modelImpl_memb, H5std_string output_name, \
        std::string subgroup_name, std::string dataset_name, \
        int numStructs, int numModels, model_t* modelList);

  /**
   * Write a list of the transactions in the simulation
   *
   */
  void writeTransList();

  /**
   * Write a list of the material histories in the simulation
   *
   */
  void writeMatHist();

  /**
   * Prepares file and memory dataspaces for homogeneous data 
   * 
   * @param dsname is the name of the dataset/dataspace
   * @param type is the type of data of the homogenous dataspace
   * @param memspace is a reference to the memory space
   * @param filespace is a reference to the file space
   * @param dataset is the prepared, selected dataset
   */
  void prepareSpaces(std::string dsname, DataType type, DataSpace &memspace, 
      DataSpace &filespace, DataSet &dataset);

  /**
   * Puts a one dimensional vector of integer data in the indicated 
   * dataspace
   *
   * @param data is the data
   * @param dsname is the dataspace
   */

  void writeData(intData1d data, std::string dsname);

  /**
   * Puts a one dimensional vector of integer data in the indicated 
   * dataspace
   *
   * @param data is the data
   * @param dsname is the dataspace
   */
  void writeData(intData2d data, std::string dsname);

  /**
   * Puts a one dimensional vector of integer data in the indicated 
   * dataspace
   *
   * @param data is the data
   * @param dsname is the dataspace
   */
  void writeData(intData3d data, std::string dsname);

  /**
   * Puts a one dimensional vector of double data in the indicated 
   * dataspace
   *
   * @param data is the data
   * @param dsname is the dataspace
   */
  void writeData(dblData1d data, std::string dsname);

  /**
   * Puts a two dimensional vector of double data in the indicated 
   * dataspace
   *
   * @param data is the data
   * @param dsname is the dataspace
   */
  void writeData(dblData2d data, std::string dsname);

  /**
   * Puts a three dimensional vector of double data in the indicated 
   * dataspace
   *
   * @param data is the data
   * @param dsname is the dataspace
   */
  void writeData(dblData3d data, std::string dsname);

  /**
   * Puts a one dimensional vector of std::string data in the indicated 
   * dataspace
   *
   * @param data is the data
   * @param dsname is the dataspace
   */
  void writeData(strData1d data, std::string dsname);

  /**
   * Puts a two dimensional vector of std::string data in the indicated 
   * dataspace
   *
   * @param data is the data
   * @param dsname is the dataspace
   */
  void writeData(strData2d data, std::string dsname);

  /**
   * Puts a three dimensional vector of std::string data in the indicated 
   * dataspace
   *
   * @param data is the data
   * @param dsname is the dataspace
   */
  void writeData(strData3d data, std::string dsname);

  /**
   * Reads 1d integer data from the dataset indicated.
   *
   * @param name is the hyperspace to read out
   * @param out_data is the vector to write to
   */
  void readData(std::string name, intData1d& out_data);

  /**
   * Reads 2d integer data from the dataset indicated.
   *
   * @param name is the name of the dataset
   * @param out_data is the vector to write to
   */
  void readData(std::string name, intData2d& out_data);

  /**
   * Reads 3d integer data from the dataset indicated.
   *
   * @param name is the name of the dataset
   * @param out_data is the vector to write to
   */
   void readData(std::string name, intData3d& out_data);
  /**
   * Reads data from the dataset indicated.
   *
   * @param name is the hyperspace to read out
   * @param out_data is the vector to write to
   */
  void readData(std::string name, dblData1d& out_data);

  /**
   * Reads data from the dataset indicated.
   *
   * @param name is the name of the dataset
   * @param out_data is the vector to write to
   */
  void readData(std::string name, dblData2d& out_data);

  /**
   * Reads data from the dataset indicated.
   *
   * @param name is the name of the dataset
   * @param out_data is the vector to write to
   */
   void readData(std::string name, dblData3d& out_data);

  /**
   * Reads data from the dataset indicated.
   *
   * @param name is the name of the dataset
   * @param out_data is the vector to write to
   */
  void readData(std::string name, strData1d& out_data);

  /**
   * Reads data from the dataset indicated.
   *
   * @param name is the name of the dataset
   * @param out_data is the vector to write to
   */
  void readData(std::string name, strData2d& out_data);

  /**
   * Reads data from the dataset indicated.
   *
   * @param name is the name of the dataset
   * @param out_data is the vector to write to
   */
   void readData(std::string name, strData3d& out_data);

  /**
   * Prints all members of a transaction.
   *
   * @param trans transaction
   */
   void printTrans(trans_t trans);

};

#endif
