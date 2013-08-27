// Regionmodel.h
#if !defined(_REGIONMODEL_H)
#define _REGIONMODEL_H

#include <set>

#include "time_agent.h"
#include "communicator.h"
#include "query_engine.h"

namespace cyclus {

/**
   @class RegionModel

   The RegionModel class is the abstract class/interface used by all
   region models

   This is all that is known externally about Regions

   @section intro Introduction
   The RegionModel type assists in defining the
   region-institution-facility hierarchy in Cyclus. A RegionModel region
   is an actor associated with a set of institutions or facilities for
   which it is responsible. A RegionModel may be used to help
   MarketModel implementations to make material routing decisions based
   on interfacility relationships. Deployment is a primary
   differentiator between different RegionModel implementations.

   Like all model implementations, there are a number of implementations
   that are distributed as part of the core Cyclus application as well
   as implementations contributed by third-party developers. The links
   below describe additional parameters necessary for the complete
   definition of a region of that implementation.

   @section functionality Basic Functionality
   All regions perform three major functions:

   -# Schedule the deployment of facilities by either
   -# Determining when new facilities need to be built, or
   -# Deferring to an InstModel to make this determination.
   -# Manage the deployment of facilities by interacting with the
   Institutions to select a specific facility type and facility
   parameters -# Passing material offers/requests between a prescribed
   market and related facilities.

   Different regional types will be required to fully define the first
   two functions while the third is a built-in capability for all region
   types. For instance, one may wish to include a region which has
   exponential growth as its driving factor for facility creation or one
   may wish to have pre-determined building order based on time step
   (e.g. the JAEA benchmark). Additionally, one may wish for there to be
   a one-to-one region-to-instituion deployment for simple models and
   thus demand that each instiution simply build a facility when its
   region determines the facility's necessity. However, one may instead
   wish to have two competing instiutions in one region and have the
   institution which provides the best incentive to the region to build
   a required facility.
   @section availableCoreImpl Available Core Implementation
   - NullRegion: This region is associated with a set of allowed
   facilities. It defers to an institution regarding facility
   deployment, making no demands on facility type or parameter (save the
   facility's allowability in the region). It makes no alterations to
   messages passed through it in either the up or down direction.
 */
class RegionModel : public TimeAgent, public Communicator {
  /* --------------------
   * all MODEL classes have these members
   * --------------------
   */
 public:
  /**
     Default constructor for RegionModel Class
   */
  RegionModel(Context* ctx);

  /**
     RegionModels should not be indestructible.
   */
  virtual ~RegionModel() {};

  /**
     Initalize the InstModel from a QueryEngine. Calls the init function.

     @param qe A pointer to a QueryEngine object containing initialization data
   */
  virtual void InitCoreMembers(QueryEngine* qe);

  /**
     perform actions required when entering the simulation
   */
  virtual void EnterSimulationAsCoreEntity();

  /**
     every model should be able to print a verbose description
   */
  virtual std::string str();

 public:
  /**
     default RegionModel receiver is to ignore messages
   */
  virtual void ReceiveMessage(Message::Ptr msg);

  /**
     Each region is prompted to do its beginning-of-time-step
     stuff at the tick of the timer.
     The default behavior is to ignore the tick.

     @param time is the time to perform the tick
   */
  virtual void HandleTick(int time);

  /**
     Each region is prompted to do its end-of-time-step
     stuff at the tock of the timer.
     The default behavior is to ignore the tock.

     @param time is the time to perform the tock
   */
  virtual void HandleTock(int time);

  /**
     Each region is prompted to do its daily task.

     @param time is the month since the start of the simulation
     @param day is the current day of that month
   */
  virtual void HandleDailyTasks(int time, int day);

 public:
  /**
     returns if the facility is in this region's allowed facs
   */
  bool IsAllowedFacility(Model* test_fac) {
    return (allowedFacilities_.find(test_fac)
            != allowedFacilities_.end());
  } ;

 protected:
  /**
     populate the region's list of allowed facilities
   */
  virtual void InitAllowedFacilities(QueryEngine* qe);

  /**
     populate the region's list of institution names
   */
  virtual void InitInstitutionNames(QueryEngine* qe);

  /**
     set the parameters necessary for RegionModel to interact
     with the simulation
   */
  virtual void AddRegionAsRootNode();

  /**
     populate the region's list of child institutions
   */
  virtual void AddChildrenToTree();

  /**
     every region has a list of allowed facilities
   */
  std::set<Model*> allowedFacilities_;

  /**
     the names of the institutions in this region
   */
  std::set<std::string> inst_names_;
};
} // namespace cyclus
#endif
