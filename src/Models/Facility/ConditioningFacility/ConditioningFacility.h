// ConditioningFacility.h
#if !defined(_CONDITIONINGFACILITY_H)
#define _CONDITIONINGFACILITY_H

#include <iostream>
#include "Logger.h"
#include <queue>
#include <string>
#include <vector>
#include <map>
#include "H5Cpp.h"
#include "hdf5.h"

#include "FacilityModel.h"
using namespace H5;


/**
  \class ConditioningFacility
  \brief This facility accepts material that must be conditioned into a waste form. 
 
  The ConditioningFacility class inherits from the FacilityModel class and 
  is dynamically loaded by the Model class when requested.
  
  \section intro Introduction

  Place an introduction to the model here. 

  \section modelparams Model Parameters

  Place a description of the required input parameters which define the model 
  implementation.

  - string fileformat : Currently supported formats for wasteform tables include hdf5 
    and csv
  - string datafile : The filename of the file containing the wasteform tables

  \section optionalparams Optional Parameters

  none

  \section detailed Detailed Behavior

  Place a description of the detailed behavior of the model. Consider describing 
  the behavior at the tick and tock as well as the behavior upon sending and
  receiving materials and messages. 

  \subsection tick handleTick

  On the tick the ConditioningFacility makes requests for each  of the waste
  commodities (streams) that its table addresses. It also makes offers of any 
  conditioned waste it contains.

  \subsection tock handleTock

  On the tock the ConditioningFacility first prepares transactions by preparing 
  and sending material from its stocks all material in its stocks up to its 
  monthly processing  capacity.

  \subsection receivemessage receiveMessage

  The ConditioningFacility throws an exception, as it doesn't deal with messages.

  \subsection removeResource removeResource

  Sends the material from the inventory to fulfill transactions. 

  \subsection addResource addResource

  Puts the material it has recieved in the stocks, to be conditioned on the tick.

 */
class ConditioningFacility : public FacilityModel  
{
/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

public:
  /**
   * Default constructor for ConditioningFacility Class
   */
  ConditioningFacility();

  /**
   * every model should be destructable
   */
  ~ConditioningFacility();
    
  /**
   * every model needs a method to initialize from XML
   *
   * @param cur is the pointer to the model's xml node 
   */
  virtual void init(xmlNodePtr cur);
  
  /**
   * every model needs a method to copy one object to another
   *
   * @param src is the ConditioningConditioning to copy
   */
  virtual void copy(ConditioningFacility* src) ;

  /**
   * This drills down the dependency tree to initialize all relevant parameters/containers.
   *
   * Note that this function must be defined only in the specific model in question and not in any 
   * inherited models preceding it.
   *
   * @param src the pointer to the original (initialized ?) model to be copied
   */
  virtual void copyFreshModel(Model* src);

  /**
   * every model should be able to print a verbose description
   */
   virtual void print();

  /**
   * @brief Transacted resources are extracted through this method
   * 
   * @param order the msg/order for which resource(s) are to be prepared
   * @return list of resources to be sent for this order
   *
   */ 
  virtual std::vector<Resource*> removeResource(Message* order);

  /**
   * Transacted resources are received through this method
   *
   * @param trans the transaction to which these resource objects belong
   * @param manifest is the set of resources being received
   */ 
  virtual void addResource(Transaction trans,
                              std::vector<Resource*> manifest);

/* ------------------- */ 

/* --------------------
 * all COMMUNICATOR classes have these members
 * --------------------
 */
public:
    /**
     * The ConditioningFacility should ignore incoming messages
     */
    virtual void receiveMessage(Message* msg);

/* -------------------- */

/* --------------------
 * all FACILITYMODEL classes have these members
 * --------------------
 */

public:

    /**
     * The handleTick function specific to the ConditioningFacility.
     *
     * @param time the time of the tick
     */
    virtual void handleTick(int time);

    /**
     * The handleTick function specific to the ConditioningFacility.
     *
     * @param time the time of the tock
     */
    virtual void handleTock(int time);

protected:

/* ------------------- */ 

/* --------------------
 * _THIS_ FACILITYMODEL class has these members
 * --------------------
 */
    /**
     * Defines the structure of data associated with each row entry in the 
     * loading database. 
     */
    typedef struct stream_t{
      int streamID; /**< an integer indicating the id of the waste stream >**/
      int formID; /**< an integer indicating the id of the waste form >**/
      double density; /**< a double indicating the maximum loading density >**/
      double wfvol; /**< a double indicating the canonical waste form volume >**/
      double wfmass; /**< a double indicating the mass of the not-loaded waste form >**/
    } stream_t;
 
    /**
     * The integer length (number of rows) of the datafile/med/loading/ dataset 
     */
    int stream_len_;

    /**
     * The vector of stream structs that holds the data in the datafile instance.
     */
    std::vector<stream_t> stream_vec_;

    /// the stocks are where the processed material is kept
    std::deque<std::pair<std::string, Material*> > stocks_;

    /// the inventory is where the processed material is kept
    std::deque<Material*> inventory_;

    /// a map from format names to table loading function pointers
    std::map<std::string, void(ConditioningFacility:: *)(std::string)> allowed_formats_;

    /// is the datafile open?
    bool file_is_open_;

    /// Loading density table. Rows are waste streams, columns are waste forms
    boost::multi_array<double, 2> loading_densities_;

    /**
     * loads the table from a file of filetype type
     *
     * @param name the name of the file to load
     * @param type the type of file to load
     */
    void loadTable(std::string name, std::string type);

    /**
     * verifies that the table is an allowed type
     *
     * @param name the name of the file to load
     * @param type the type of file to load
     */
    bool verifyTable(std::string name, std::string type);

    /**
     * loads the hdf5 file into a table of waste streams and forms
     *
     * @param name the name of the file to load
     */
    void loadHDF5File(std::string name);

    /**
     * loads the sql file into a table of waste streams and forms
     *
     * @param name the name of the file to load
     */
    void loadSQLFile(std::string name);

    /**
     * loads xml file (pointer?) into a table of waste streams and forms
     *
     * @param name the name of the file to load
     */
    void loadXMLFile(std::string name);

    /**
     * loads csv file into a table of waste streams and forms
     *
     * @param name the name of the file to load
     */
    void loadCSVFile(std::string name);

    /**
     * Requests each of the commodities it can accept and condition
     * at a level that corresponds to its monthly capacity
     */
    void makeRequests();

    /**
     * Offers each of the conditioned streams in its inventory
     */
    void makeOffers();

    /**
     * conditions all the materials it is capable of conditioning this
     * month.
     */
    void conditionMaterials();

    /**
     * processes orders on the orders_waiting_ stack and sends the materials
     */
    void processOrders();


/* --------------------
   output directory info
 * --------------------
 */
 public:
  /**
     The getter function for this facility model output dir
  */
  static std::string outputDir(){ 
    return FacilityModel::outputDir().append(outputDir_);}

 private:
  /**
     Every specific facility model writes to the output database
     location: FacilityModel::OutputDir_ + /this_facility's_handle
  */
  static std::string outputDir_;

/* ------------------- */ 

};

#endif

