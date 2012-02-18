// BookKeeper.h
#if !defined(_BOOKKEEPER_H)
#define _BOOKKEEPER_H

#include <string>
#include <vector>

#include "H5Cpp.h"
#include "hdf5.h"

#include "Model.h"
#include "Material.h"
#include "Message.h"
#include "CycException.h"
#include "boost/any.hpp"

#define BI BookKeeper::Instance()
#define NUMISOS 1500

typedef std::map<std::string, boost::any> ParamMap;

using namespace H5;

/*!
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

  static int next_comp_entry_id_;
  
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
  
  /// True iff the db is open.
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
  typedef struct agent_t{
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
  } agent_t;
  
  // transaction struct
  typedef struct trans_t{
    int transID;            /**< 
                               An integer indicating the transaction ID **/
    int supplierID;         /**< 
                               An integer indicating the supplier model ID **/
    int requesterID;        /**< 
                               An integer indicating the requester model ID **/
    int timestamp;          /**< 
                               An integer indicating the month **/
    double price;           /**< 
                               A double indicating the transaction price **/   
    char commodName[64];   /**< 
                              The name of the commodity **/
  } trans_t;
  
  // material history struct
  typedef struct mat_hist_t{
    int stateID;         /**< 
                               An integer indicating the material state ID **/
    int materialID;         /**< 
                               An integer indicating the material object ID **/
    int transID;         /**< 
                               An integer indicating the associated transaction ID **/
    int timestamp;          /**< 
                               An integer indicating the timestamp **/
    double quantity;          /**< 
                               An double indicating the resource quantity **/
    char units[64];          /**< 
                               An string indicating the units of the resource's quantity **/
    char name[64];          /**< 
                               An string indicating the units of the resource's quantity **/
    int iso[NUMISOS];       /**< 
                               An integer indicating the nuclide ID **/   
    double comp[NUMISOS];   /**< 
                               The kg or moles of the iso in the material at 
                               that time **/
  } mat_hist_t;

  // material composition history struct
  typedef struct comp_entry_t {
    int entryID;         /**< 
                               An integer indicating the comp entry ID **/
    int stateID;         /**< 
                               An integer indicating the material state ID **/
    int iso;       /**< 
                               An integer indicating the nuclide ID **/   
    double comp;   /**< 
                               The kg or moles of the iso in the material at 
                               that time **/
  } comp_entry_t;

  /**
   * Stores the transactions that have taken place during the simulation.
   */
  std::vector<trans_t> transactions_;

  /**
   * Stores the material changes that have taken place during the simulation.
   */
  std::vector<mat_hist_t> materials_;

  /**
   * Stores model/agent data
   */
  std::map<int, ParamMap> agent_data_;

  
  /**
   * Stores the material changes that have taken place during the simulation.
   */
  std::vector<comp_entry_t> comp_entries_;

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
     returns a pair of strings 
     1) the output group and 
     2) the data sub group
     
     @param output_dir the full output directory for the subgroup
   */
  std::pair <std::string, std::string> getGroupNamePair(std::string output_dir);

  /**
   * Register the transaction in the BookKeeper's map of transactions
   *
   * @param id the transaction ID.
   * @param msg the message containing the transaction
   * @param manifest a vector the materials fulfilling this transaction
   */
  void registerTransaction(int id, msg_ptr msg, std::vector<rsrc_ptr> manifest);

  /**
   * Register the resource in the BookKeeper's map of material changes
   *
   * @param trans_id the transaction ID associated with this resource state
   * @param resource the resource who's state is to be recorded
   */
  void registerResourceState(int trans_id, rsrc_ptr resource);

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
     Write a dataset given required information. Based on the input of
     HDF5's DataSet class' write function.

     @param data buffer containing data to be written
     @param data_desc hdf5 DataType describing the data
     @param data_rank rank of the HDF5 DataSpace of the data to be written
     @param data_dims dimension of the HDF5 DataSpace of the data to be written
     @param dataset_name the name of the dataset (e.g. agentList)
     @param output_dir the final output directory for the dataset (e.g. /output/agent)
   */
  void writeDataSet(const void *data, const DataType &data_desc, 
                    int data_rank, const hsize_t *data_dims,
                    std::string dataset_name, std::string output_dir);

  /**
   * Write generic Agent information from the simulation
   *
   */
  void writeAgentList();

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

  void writeMatComps(Group* subgroup);

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

  template <class T>
  void registerModelDatum(int id, std::string param, T val) {
    ParamMap curr_agent;
    if (agent_data_.count(id) != 0) {
      curr_agent = agent_data_[id];
    }
    boost::any item = val;
    curr_agent[param] = item;
    agent_data_[id] = curr_agent;
  }

  template <class T>
  T modelDatum(int id, std::string param) {
    if (agent_data_.count(id) == 0) {
      throw CycIndexException("That agent ID doesn't exist.");
    }

    ParamMap curr_agent = agent_data_[id];

    if (curr_agent.count(param) == 0) {
      throw CycIndexException("That agent param doesn't exist.");
    }

    return boost::any_cast<T>(curr_agent[param]);
  }

};

#endif

