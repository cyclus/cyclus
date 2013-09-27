// facility_model.h
#if !defined(_FACILITYMODEL_H)
#define _FACILITYMODEL_H

#include <string>
#include <vector>

#include "model.h"
#include "time_agent.h"
#include "communicator.h"

namespace cyclus {

// forward declare Material class to avoid full inclusion and dependency
class Material;
class InstModel;

/**
   @class FacilityModel
   The FacilityModel class is the abstract class/interface used by all
   facility models

   This is all that is known externally about facilities

   @section intro Introduction

   The FacilityModel type plays a primary role in Cyclus.  A
   FacilityModel facility is where offers and requests are generated and
   transmitted to markets and where shipments of material, issued by the
   markets, are executed. The algorithms to determine what offers and
   requests are issued and how material shipments are handled are the
   primary differentiators between different FacilityModel
   implementations.
   Like all model implementations, there are a number of implementations
   that are distributed as part of the core Cyclus application as well
   as implementations contributed by third-party developers.  The links
   below describe additional parameters necessary for the complete
   definition of a facility of that implementation.

   @section available Available Core Implementations

   - SourceFacility: A facility that can produce a constant (or
   infinite) amount of some commodity
   - SinkFacility:  A facility that can consume a constant (or infinite)
   amount of some commodity
   - NullFacility: A facility that consumes a constant amount of one
   commodity and produces a constant amount of another


   @section anticipated Anticipated Core Implementations

   Developers are encouraged to add to this list and create pages that
   describe the detailed behavior of these models.

   - RecipeReactor: A facility that consumes a fixed fresh fuel recipe
   one a time scale governed by reactor cycle lengths and batch sizes,
   and produced a fixed/corresponding spent fuel recipe at the same
   frequency - SeparationsMatrixFacility: A facility that consumes a
   fixed quantity of material of one commodity and produces many
   different output streams with the input isotopes distributed across
   those output streams according to a fixed matrix
   - EnrichmentFacility: A facility that offers a fixed quantity of SWUs
   to accomplish isotopic enrichment of material

   @section thirdparty Third-party Implementations

   Collaborators are encouraged to add to this list and link to external
   pages that describe how to get the models and the detailed behavior
 */

class FacilityModel : public TimeAgent, public Communicator {
  /* --------------------
   * all MODEL classes have these members
   * --------------------
   */
 public:
  FacilityModel(Context* ctx);

  virtual ~FacilityModel();

  /**
     Initalize the FacilityModel from xml. Calls the init function.
     @param qe a pointer to a QueryEngine object containing intialization data
   */
  virtual void InitCoreMembers(QueryEngine* qe);

  /**
     Copy module members from a source model
     @param source the model to copy from
   */
  virtual void InitFrom(Model* m);

  virtual void Deploy(Model* parent) {
    Model::Deploy(parent);
  }

  /**
     every model should be able to print a verbose description
   */
  virtual std::string str();

  /* ------------------- */


  /* --------------------
   * all COMMUNICATOR classes have these members
   * --------------------
   */
 public:
  /**
     There is no default FacilityModel receiver

     Each derived class must implement an offer/request receiver
   */
  virtual void ReceiveMessage(Message::Ptr msg) = 0;

  /* ------------------- */


  /* --------------------
   * all FACILITYMODEL classes have these members
   * --------------------
   */
 protected:
  /**
     each facility should have an institution that manages it
   */
  std::string inst_name_;

  /**
     Most facilities will have a vector of incoming, request commodities.
     Ultimately, it's up to the facility to utilize this list. However, the
     user interface is assisted by this specificity in the input scheme.
     For details, see issue #323 in cyclus/cyclus.
   */
  std::vector<std::string> in_commods_;

  /**
     most facilities will have a vector of outgoing, offer commodities
     Ultimately, it's up to the facility to utilize this list. However, the
     user interface is assisted by this specificity in the input scheme.
     For details, see issue #323 in cyclus/cyclus.
   */
  std::vector<std::string> out_commods_;

  /**
     each facility needs a lifetime
   */
  int fac_lifetime_;

  /**
     decommissions the facility, default behavior is for the facility
     to delete itself
   */
  virtual void Decommission();

  /**
     facilities over write this method if a condition must be met
     before their destructors can be called
   */
  virtual bool CheckDecommissionCondition();

 public:
  /**
     Sets the facility's name
     @param facName is the new name of the facility
   */
  virtual void SetFacName(std::string facName) {
    this->SetName(facName);
  };

  /**
     Returns the facility's name
     @return fac_name_ the name of this facility, a string
   */
  virtual std::string FacName() {
    return this->name();
  };

  /**
     Sets this facility's instutution name
     @param name the name of the institution associated with this
   */
  virtual void SetInstName(std::string name) {
    inst_name_ = name;
  };

  /**
     Returns this facility's institution
     @return the institution assosicated with this facility
   */
  virtual InstModel* FacInst();

  /**
     Sets the facility's lifetime
     @param lifetime is the new lifetime of the facility in months
   */
  virtual void SetFacLifetime(int lifetime) {
    fac_lifetime_ = lifetime;
  };

  /**
     Returns the facility's lifetime
     @return fac_lifetime_ the lifetime of this facility, an int, in
   */
  virtual int FacLifetime() {
    return fac_lifetime_;
  };

  /**
     @return the input commodities
  */
  std::vector<std::string> InputCommodities();

  /**
     @return the output commodities
  */
  std::vector<std::string> OutputCommodities();

  /**
     @param time the time used to query whether it is past the
     facility's decommission date
     @return true if the time is greater than the decommission date
   */
  bool LifetimeReached(int time);

  /**
     Each facility is prompted to do its beginning-of-time-step
     stuff at the tick of the timer.

     @param time is the time to perform the tick
   */
  virtual void HandleTick(int time) = 0;

  /**
     Each facility is prompted to its end-of-time-step
     stuff on the tock of the timer.

     By default, facilities are decommissioned if time is greater
     than or equal to its end date.

     @param time is the time to perform the tock
   */
  virtual void HandleTock(int time) = 0;

  /**
     Each facility is prompted to do its daily tasks

     @param time is the number of months since the beginning of the
     simulation @param day is the current day in this month
   */
  virtual void HandleDailyTasks(int time, int day);

  friend class InstModel;
  /* ------------------- */

};
} // namespace cyclus

#endif
