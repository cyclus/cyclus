// FacilityModel.h
#if !defined(_FACILITYMODEL_H)
#define _FACILITYMODEL_H

#include <string>
#include <vector>

#include "TimeAgent.h"
#include "Communicator.h"
#include "InstModel.h"

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
class FacilityModel : public TimeAgent, public Communicator {
/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

public:
  /// Default constructor for FacilityModel Class
  FacilityModel() {
    setModelType("Facility");
  };

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

public:
  /**
   *  There is no default FacilityModel receiver 
   *
   *  Each derived class must implement an offer/request receiver
   */ 
  virtual void receiveMessage(Message* msg)=0;

protected:
  /// each facility should have an institution that manages it
  //Model* inst;
  std::string inst_name_;

  /// each instance of a facility needs a name
  std::string fac_name_;

  /// each facility needs a lifetime
  int fac_lifetime_;

  /**
   * There is no default FacilityModel shipment requester 
   * Each derived class must implement a shipment requester
   *
   * @param trans is the transaction being executed
   * @param manifest is the set of materials being received
   *
   */ 
  virtual void receiveMaterial(Transaction trans, std::vector<Material*> manifest) = 0;
  

public:

  /**
   * Sets the facility's name 
   *
   * @param facName is the new name of the facility
   */
  void setFacName(std::string facName) { fac_name_ = facName; };

  /**
   * Returns the facility's name
   *
   * @return fac_name_ the name of this facility, a string
   */
  std::string getFacName() { return fac_name_; };

  /**
   * Sets this facility's instutution name 
   *
   * @param name the name of the institution associated with this facility.
   */
  void setInstName(std::string name){ inst_name_ = name;};

  /**
   * Returns this facility's institution
   *
   * @return the institution assosicated with this facility
   */
  InstModel* getFacInst();

  /**
   * Sets the facility's lifetime 
   *
   * @param lifetime is the new lifetime of the facility in months
   */
  void setFacLifetime(int lifetime) { fac_lifetime_ = lifetime; };

  /**
   * Returns the facility's lifetime
   *
   * @return fac_lifetime_ the lifetime of this facility, an int, in 
   * months
   */
  int getFacLifetime() { return fac_lifetime_; };

  /**
   * Returns the facility's power capacity
   *
   * @return 0 by default. If the facility produces power, it will use its own function.
   */
  double getPowerCapacity() { return 0.0; };

  /**
   * Registers the transaction with the BookKeeper and sends the material
   *
   * @param order contains the order being executed
   * @param manifest is the set of materials being received
   */ 
  virtual void sendMaterial(Message* order, std::vector<Material*> manifest);
  
  /**
   * Each facility is prompted to do its beginning-of-life
   * stuff.
   *
   */
  virtual void handlePreHistory();

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



