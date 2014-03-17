// facility.h
#ifndef CYCLUS_FACILITY_H_
#define CYCLUS_FACILITY_H_

#include <string>
#include <vector>
#include <set>

#include "agent.h"
#include "time_listener.h"
#include "trader.h"

namespace cyclus {

// forward declare Material class to avoid full inclusion and dependency
class Material;
class Institution;

/**
   @class Facility
   The Facility class is the abstract class/interface used by all
   facility agents

   This is all that is known externally about facilities

   @section intro Introduction

   The Facility type plays a primary role in Cyclus.  A Facility
   facility is where offers and requests are generated and transmitted to a
   ResourceExchange and where shipments of material, issued by the exchange, are
   executed. The algorithms to determine what offers and requests are issued and
   how material shipments are handled are the primary differentiators between
   different Facility implementations.

   Like all agent implementations, there are a number of implementations that
   are distributed as part of the core Cyclus application as well as
   implementations contributed by third-party developers.  The links below
   describe additional parameters necessary for the complete definition of a
   facility of that implementation.

   @section available Available Core Implementations

   - SourceFacility: A facility that can produce a constant (or
   infinite) amount of some commodity
   - SinkFacility:  A facility that can consume a constant (or infinite)
   amount of some commodity
   - NullFacility: A facility that consumes a constant amount of one
   commodity and produces a constant amount of another


   @section anticipated Anticipated Core Implementations

   Developers are encouraged to add to this list and create pages that
   describe the detailed behavior of these agents.

   - RecipeReactor: A facility that consumes a fixed fresh fuel recipe
   one a time scale governed by reactor cycle lengths and batch sizes,
   and produced a fixed/corresponding spent fuel recipe at the same
   frequency - SeparationsMatrixFacility: A facility that consumes a
   fixed quantity of material of one commodity and produces many
   different output streams with the input nuclides distributed across
   those output streams according to a fixed matrix
   - EnrichmentFacility: A facility that offers a fixed quantity of SWUs
   to accomplish enrichment of material

   @section thirdparty Third-party Implementations

   Collaborators are encouraged to add to this list and link to external
   pages that describe how to get the agents and the detailed behavior
 */
class Facility : public TimeListener, public Agent, public Trader {
 public:
  Facility(Context* ctx);

  virtual ~Facility();

  // DO NOT call Agent class implementation of this method
  virtual void InfileToDb(InfileTree* qe, DbInit di) {};

  // DO NOT call Agent class implementation of this method
  virtual void InitFrom(QueryableBackend* b) {}

  // DO NOT call Agent class implementation of this method
  virtual void Snapshot(DbInit di) {}

  /**
     Copy module members from a source agent

     Any facility subclassing facility agent should invoke their own InitFrom
     method, calling Facility's first!
     
     @param m the agent to copy from
   */
  void InitFrom(Facility* m);

  /**
     @brief builds the facility in the simulation

     @param parent the parent of this facility
   */
  virtual void Build(Agent* parent = NULL);

  virtual void DoRegistration();

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

  /**
     every agent should be able to print a verbose description
   */
  virtual std::string str();
};

} // namespace cyclus

#endif // ifndef CYCLUS_FACILITY_H_
