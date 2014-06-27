#ifndef CYCLUS_SRC_REGION_H_
#define CYCLUS_SRC_REGION_H_

#include <set>

#include "time_listener.h"
#include "infile_tree.h"

namespace cyclus {

/// @class Region
///
/// The Region class is the abstract class/interface used by all
/// region agents
///
/// This is all that is known externally about Regions
///
/// @section intro Introduction
/// The Region type assists in defining the region-institution-facility
/// hierarchy in Cyclus. A Region region is an actor associated with a set
/// of institutions or facilities for which it is responsible. A Region may
/// be used to adjust preferences in the ResourceExchange to make material
/// routing decisions based on interfacility relationships. Deployment is a
/// primary differentiator between different Region implementations.
///
/// Like all agent implementations, there are a number of implementations
/// that are distributed as part of the core Cyclus application as well
/// as implementations contributed by third-party developers. The links
/// below describe additional parameters necessary for the complete
/// definition of a region of that implementation.
///
/// @section functionality Basic Functionality
/// All regions perform three major functions:
///
/// -# Schedule the deployment of facilities by either
/// -# Determining when new facilities need to be built, or
/// -# Deferring to an Institution to make this determination.
/// -# Manage the deployment of facilities by interacting with the
/// Institutions to select a specific facility type and facility
/// parameters
///
/// Different regional types will be required to fully define the first
/// two functions while the third is a built-in capability for all region
/// types. For instance, one may wish to include a region which has
/// exponential growth as its driving factor for facility creation or one
/// may wish to have pre-determined building order based on time step
/// (e.g. the JAEA benchmark). Additionally, one may wish for there to be
/// a one-to-one region-to-instituion deployment for simple agents and
/// thus demand that each instiution simply build a facility when its
/// region determines the facility's necessity. However, one may instead
/// wish to have two competing instiutions in one region and have the
/// institution which provides the best incentive to the region to build
/// a required facility.
/// @section availableCoreImpl Available Core Implementation
/// - NullRegion: This region is associated with a set of allowed
/// facilities. It defers to an institution regarding facility
/// deployment, making no demands on facility type or parameter (save the
/// facility's allowability in the region). It makes no alterations to
/// messages passed through it in either the up or down direction.
class Region : public Agent, public TimeListener {
  // --------------------
  // all MODEL classes have these members
  // --------------------
 public:
  /// Default constructor for Region Class
  Region(Context* ctx);

  /// Regions should not be indestructible.
  virtual ~Region() {}

  // DO NOT call Agent class implementation of this method
  virtual void InfileToDb(InfileTree* qe, DbInit di) {}

  // DO NOT call Agent class implementation of this method
  virtual void InitFrom(QueryableBackend* b) {}

  // DO NOT call Agent class implementation of this method
  virtual void Snapshot(DbInit di) {}

  virtual void InitInv(Inventories& inv) {}

  virtual Inventories SnapshotInv() { return Inventories(); }

  /// perform actions required when entering the simulation
  virtual void Build(Agent* parent);

  virtual void EnterNotify();

  virtual void Decommission();

  /// every agent should be able to print a verbose description
  virtual std::string str();

  virtual void Tick() {}

  virtual void Tock() {}

 protected:
  void InitFrom(Region* m);
};

}  // namespace cyclus

#endif  // CYCLUS_SRC_REGION_H_
