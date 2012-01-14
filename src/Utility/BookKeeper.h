// BookKeeper.h
#if !defined(_BOOKKEEPER_H)
#define _BOOKKEEPER_H

#include <string>
#include <vector>
#include "H5Cpp.h"
#include "hdf5.h"

#include "Teuchos_ParameterList.hpp"

#include "Model.h"
#include "Material.h"
#include "Message.h"

#define BI BookKeeper::Instance()
#define NUMISOS 100

using namespace H5;

/**
   @brief
   The BookKeeper is a (singleton) class for handling I/O.
   
   @section introduction Introduction
   The Cyclus Book Keeper is a singleton member of the 
   BookKeeperClass. The Book Keeper Instance (BI) controls 
   reading and writing from the simulation database. The 
   Book Keeper creates the database file (cyclus.h5) and 
   maintains functions with which the LogicianClass and 
   various models can write individual datasets to that file.
   
   @section singletonInstance Singleton Instance
   In order to utilize the database functions stewarded by the 
   Book Keeper, a model must include the Book Keeper header file 
   and call BookKeeperClass functions via the singleton Book 
   Keeper Instance LI.
   
   @section writeDB Writing the Database File
   In the long term, the function that writes the output for 
   Cyclus may be of any format. For now, the only supported format 
   is hdf5. The Book Keeper creates a simulation data file in the 
   build directory (called cyclus.h5) as soon as it is initialized.

   @section writeToDB Writing to the Database
   It is an open question whether the data for the simulation should 
   be kept entirely in memory until the end of the simulation. It 
   may be best to open and write to the file all in one fluid process 
   at the end of the simulation to save time. Alternatively though, 
   there may be some value in writing the simulation data to the file 
   as it is generated in order to facilitate a future in which the 
   simulation might have a dynamic start and stop capability.
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

  // an agent struct
  typedef struct model_t{
    int ID;                 /**< 
                               An integer indicating the agent ID **/
    char name[64];          /**< 
                               A std::string indicating the name of the 
                               template **/ 
    char modelImpl[64];     /**< 
                               A std::string indicating the agent 
                               implementation **/
    int parentID;           /**< 
                               An integer of the agent's parent's ID **/
    int bornOn;           /**< 
                               An integer of the agents born on date **/
    int diedOn;           /**< 
                               An integer of the agents died on date **/
  } model_t;
  
  // facility, market, converter, or institution model struct
  typedef struct trans_t{
    int transID;            /**< 
                               An integer indicating the transaction ID **/
    int supplierID;         /**< 
                               An integer indicating the supplier model ID **/
    int requesterID;        /**< 
                               An integer indicating the requester model ID **/
    int materialID;         /**< 
                               An integer indicating the material object ID **/
    int timestamp;          /**< 
                               An integer indicating the month **/
    double price;           /**< 
                               A double indicating the transaction price **/   
    char commodName[64];   /**< 
                              The name of the commodity **/
  } trans_t;
  
  // material history struct
  typedef struct mat_hist_t{
    int materialID;         /**< 
                               An integer indicating the material object ID **/
    int transID;         /**< 
                               An integer indicating the associated transaction ID **/
    int timestamp;          /**< 
                               An integer indicating the timestamp **/
    int iso[NUMISOS];       /**< 
                               An integer indicating the nuclide ID **/   
    double comp[NUMISOS];   /**< 
                               The kg or moles of the iso in the material at 
                               that time **/
  } mat_hist_t;
  
  /**
   * Stores the transactions that have taken place during the simulation.
   */
  std::vector<trans_t> transactions_;

  /**
   * Stores the material changes that have taken place during the simulation.
   */
  std::vector<mat_hist_t> materials_;

  /**
   * Stores the materials_ vector index of the last time a material registered
   */
  std::map<int, int> last_mat_idx_;

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
  void registerTrans(Message* msg, std::vector<Resource*> manifest);

  /**
   * Register the material in the BookKeeper's map of material changes
   *
   * @param trans_id the transaction ID associated with this material state
   * @param mat the material with a history
   */
  void registerMatState(int trans_id, Material* mat);

  /**
   * Register the materialin the BookKeeper's map of material changes
   *
   */
  void registerRepoComponent(int ID, std::string name, 
                             std::string thermalModel, 
                             std::string nuclideModel, int parentID, 
                             double innerRadius, double outerRadius, double x,
                             double y, double z);

  /**
   * Write a list of the facility/inst/market models in the simulation
   *
   */
  void writeModelList();

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
  void doModelWrite(H5std_string ID_memb, H5std_string name_memb,
		    H5std_string modelImpl_memb, 
		    H5std_string parentID_memb, H5std_string bornOn_memb,
		    H5std_string diedOn_memb, H5std_string output_name,
		    std::string subgroup_name, std::string dataset_name,
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

  /**
   * Prints all members of a transaction.
   *
   * @param trans transaction
   */
   void printTrans(trans_t trans);

};

#endif
