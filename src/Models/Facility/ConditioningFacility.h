// ConditioningFacility.h
#if !defined(_CONDITIONINGFACILITY_H)
#define _CONDITIONINGFACILITY_H

#include <iostream>
#include "Logger.h"
#include <queue>
#include <string>
#include <vector>
#include "H5Cpp.h"
#include "hdf5.h"

#include "FacilityModel.h"
using namespace H5;


/**
 * The ConditioningFacility class inherits from the FacilityModel class and is dynamically
 * loaded by the Model class when requested.
 * 
 * This facility will do nothing. This FacilityModel is intended as a skeleton to guide
 * the implementation of new FacilityModel models. 
 *
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
     * This sends material up the Inst/Region/Logician line
     * to be passed back down to the receiver
     *
     * @param order the Message object containing the order being filled
     * @param receiver the ultimate facility to receive this transaction
     */
    virtual void sendMaterial(Message* order, const Communicator* receiver);
    
    /**
     * The facility receives the materials other facilities have sent.
     *
     * @param trans the Transaction object describing the order being filled
     * @param manifest the list of material objects being received
     */
    virtual void receiveMaterial(Transaction trans, vector<Material*> manifest);

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
    /// typedef for the stream type
    typedef struct stream_t{
      string ws;
      string wf;
      double load;
    } stream_t;

    /// the stocks are where the processed material is kept
    std::deque<std::pair<Commodity*,Material*> >stocks_;

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

/* ------------------- */ 

};

#endif

