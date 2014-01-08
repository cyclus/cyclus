// inst_model.h
#ifndef CYCLUS_INST_MODEL_H_
#define CYCLUS_INST_MODEL_H_

#include "time_listener.h"
#include "model.h"
#include "region_model.h"

#include <map>
#include <set>
#include <list>

namespace cyclus {

class QueryEngine;

// Usefull Typedefs
typedef std::set<std::string> PrototypeSet;
typedef std::set<std::string>::iterator PrototypeIterator;

/**
   The InstModel class is the abstract class/interface
   used by all institution models

   @section introduction Introduction The InstModel type assists in defining the
   region-institution-facility hierarchy in Cyclus. A InstModel institution is
   an actor associated with a set of facilities for which it is responsible. An
   InstModel may be used to adjust preferences in the ResourceExchange to make
   material routing decisions based on interfacility relationships. Deployment
   is a primary differentiator between different InstModel implementations.

   Like all model implementations, there are a number of
   implementations that are distributed as part of the core Cyclus
   application as well as implementations contributed by third-party
   developers. The links below descrie parameters necessary for the
   complete definition of an implemented instituion.

   @section availableCoreImpl Available Core Implementations
   - FixedInst: This institution is unchanging and is statically
   associated with facilities that are deployed by some other
   entity (such as the region or the logician.

   @section anticipatedCoreImpl Anticipated Core Implementations
   - DeploymentInst: This institution deploys allowed facilities
   according to a demand curve.

   @section thirdPartyImpl Third Party Implementations
   (None)
 */
class InstModel : public TimeListener {
  /* --------------------
   * all MODEL classes have these members
   * --------------------
   */
 public:
  /**
     Default constructor for InstModel Class
   */
  InstModel(Context* ctx);

  /**
     every model should be destructable
   */
  virtual ~InstModel() {};

  /**
     Initalize the InstModel from a QueryEngine. Calls the init function.

     @param qe a pointer to a QueryEngine object containing intialization data
   */
  virtual void InitCoreMembers(QueryEngine* qe);

  /**
     every model should be able to print a verbose description
   */
  virtual std::string str();

 public:
  /**
     Each institution is prompted to do its beginning-of-time-step
     stuff at the tick of the timer.
     Default behavior is to ignore the tick.

     @param time is the time to perform the tick
   */
  virtual void Tick(int time);

  /**
     Each institution is prompted to its end-of-time-step
     stuff on the tock of the timer.
     Default behavior is to ignore the tock.

     @param time is the time to perform the tock
   */
  virtual void Tock(int time);

  /**
     perform all tasks required when an inst enters the simulation
   */
  virtual void Deploy(Model* parent);
  /* ------------------- */
 
  /* --------------------
   * all INSTMODEL classes have these members
   * --------------------
   */
  /**
     return the number of prototypes this inst can build
   */
  int NPrototypes() {
    return prototypes_.size();
  }

  /**
     return the first prototype
   */
  PrototypeIterator BeginPrototype() {
    return prototypes_.begin();
  }

  /**
     return the last prototype
   */
  PrototypeIterator EndPrototype() {
    return prototypes_.end();
  }

  /**
     Checks if prototype is in the prototype list
   */
  bool IsAvailablePrototype(std::string proto_name) {
    return (prototypes_.find(proto_name) != prototypes_.end());
  }

  /**
     returns this institution's region
   */
  RegionModel* GetRegion() {
    return (dynamic_cast<RegionModel*>(this->parent()));
  };

  /**
     reports number of facilities in this inst
   */
  int GetNumFacilities() {
    return children().size();
  };

  /**
     builds a prototype
     @param proto_name the name of the prototype to build
   */
  void Build(std::string proto_name);

 protected:
  void InitFrom(InstModel* m);

  /**
     add a prototoype to the set of available prototypes
     @param proto_name the name of the prototype to add
   */
  void AddAvailablePrototype(std::string proto_name);

  /**
     perform any actions required after prototype has been added to
     the list of available prototypes
     @param proto_name the name of prototype to register
   */
  virtual void RegisterAvailablePrototype(std::string proto_name);

  /**
     Adds a prototype build order to initial_build_order_
     @param qe a pointer to a QueryEngine object containing intialization data
   */
  void AddPrototypeToInitialBuild(QueryEngine* qe);

  /**
     checks if a prototype is in its list of available prototypes
     if not, it throws an error
     @param p the prototype to check for
   */
  void ThrowErrorIfPrototypeIsntAvailable(std::string p);

  /**
     perform any registration functionality after a clone has been
     built
     @param clone the built (cloned) prototype
   */
  virtual void RegisterCloneAsBuilt(Model* clone);

  /**
     perform any registration functionality before a clone is
     decommissioned(deleted)
     @param clone the to-be-decommissioned
   */
  virtual void RegisterCloneAsDecommissioned(Model* clone);

private:
  /**
     The Inst's set of available prototypes to build
   */
  PrototypeSet prototypes_;

  /**
     the initial prototypes to build
   */
  std::map<std::string, int> initial_build_order_;
  /* ------------------- */
};

} // namespace cyclus

#endif // ifndef CYCLUS_INST_MODEL_H_

