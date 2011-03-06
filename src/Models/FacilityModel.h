// FacilityModel.h
#if !defined(_FACILITYMODEL_H)
#define _FACILITYMODEL_H
#include <string>
#include <vector>

#include "Model.h"
#include "Communicator.h"
#include "InstModel.h"

using namespace std;

/// forward declaration to resolve recursion
class Commodity;
// forward declare Material class to avoid full inclusion and dependency
class Material;

//-----------------------------------------------------------------------------
/**
 * The FacilityModel class is the abstract class/interface used by all
 * facility models
 * 
 * This is all that is known externally about facilities
*/
//-----------------------------------------------------------------------------
class FacilityModel : public Model, public Communicator
{
/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

public:
  /// Default constructor for FacilityModel Class
  FacilityModel() { ID = nextID++; model_type="Facility"; commType=FacilityComm;};

  /// every model should be destructable
  virtual ~FacilityModel() {};
  
  // every model needs a method to initialize from XML
  virtual void init(xmlNodePtr cur);
  
  // every model needs a method to copy one object to another
  virtual void copy(FacilityModel* src);

  /**
   * This drills down the dependency tree to initialize all relevant parameters/containers.
   *
   * Note that this function must be defined only in the specific model in question and not in any 
   * inherited models preceding it.
   *
   * @param src the pointer to the original (initialized ?) model to be copied
   */
  virtual void copyFreshModel(Model* src)=0;

  // every model should be able to print a verbose description
  virtual void print()              { Model::print(); };

protected: 
  /// Stores the next available facility ID
  static int nextID;
/* ------------------- */ 

/* --------------------
 * all COMMUNICATOR classes have these members
 * --------------------
 */
public:
  /**
   *  There is no default FacilityModel receiver 
   *
   *  Each derived class must implement an offer/request receiver
   */ 
  virtual void receiveMessage(Message* msg)=0;

protected:


/* ------------------- */ 


/* --------------------
 * all FACILITYMODEL classes have these members
 * --------------------
 */
protected:
  /// each facility should have an institution that manages it
  //Model* inst;
  string inst_name;

  /// each instance of a facility needs a name
  string fac_name;

  /// each facility needs a start construction date
  int fac_start;

  /// each facility needs an end decommisioning date
  int fac_end;

public:

  /**
   * Sets the facility's name 
   *
   * @param facName is the new name of the facility
   */
  void setFacName(string facName) { fac_name = facName; };

  /**
   * Returns the facility's name
   *
   * @return fac_name the name of this facility, a string
   */
  string getFacName() { return fac_name; };

  /**
   * Sets this facility's instutution name 
   *
   * @param name the name of the institution associated with this facility.
   */
  void setInstName(string name){ inst_name = name;};

  /**
   * Returns this facility's institution
   *
   * @return the institution assosicated with this facility
   */
  InstModel* getFacInst();

  /**
   * There is no default FacilityModel shipment requester 
   * Each derived class must implement a shipment requester
   *
   * @param trans is the transaction being executed
   * @param manifest is the set of materials being received
   *
   */ 
  virtual void receiveMaterial(Transaction trans, vector<Material*> manifest) = 0;
  
  /**
   * Each facility is prompted to do its beginning-of-time-step
   * stuff at the tick of the timer.
   *
   * @param time is the time to perform the tick
   */
  virtual void handleTick(int time);

  /**
   * Each facility is prompted to its end-of-time-step
   * stuff on the tock of the timer.
   * 
   * @param time is the time to perform the tock
   */
  virtual void handleTock(int time);

/* ------------------- */ 
  
};

#endif



