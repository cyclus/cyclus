#ifndef CYCLUS_SRC_INSTITUTION_H_
#define CYCLUS_SRC_INSTITUTION_H_

#include <list>
#include <map>
#include <set>

#include "agent.h"
#include "region.h"
#include "time_listener.h"

namespace cyclus {

class InfileTree;

// Usefull Typedefs
typedef std::set<std::string> PrototypeSet;
typedef std::set<std::string>::iterator PrototypeIterator;

/// The Institution class is the abstract class/interface
/// used by all institution agents
///
/// @section introduction Introduction The Institution type assists in defining
/// the region-institution-facility hierarchy in Cyclus. A Institution
/// institution is an actor associated with a set of facilities for which it is
/// responsible. An Institution may be used to adjust preferences in the
/// ResourceExchange to make material routing decisions based on interfacility
/// relationships. Deployment is a primary differentiator between different
/// Institution implementations.
class Institution : public Agent, public TimeListener {
  // --------------------
  // all MODEL classes have these members
  // --------------------
 public:
  /// Default constructor for Institution Class
  Institution(Context* ctx);

  /// every agent should be destructable
  virtual ~Institution();

  // DO NOT call Agent class implementation of this method
  virtual void InfileToDb(InfileTree* qe, DbInit di) {}

  // DO NOT call Agent class implementation of this method
  virtual void InitFrom(QueryableBackend* b) {}

  // DO NOT call Agent class implementation of this method
  virtual void Snapshot(DbInit di) {}

  virtual void InitInv(Inventories& inv) {}

  virtual Inventories SnapshotInv() { return Inventories(); }

  /// every agent should be able to print a verbose description
  virtual std::string str();

  /// perform all tasks required when an inst enters the simulation
  virtual void Build(Agent* parent);

  virtual void EnterNotify();

  virtual void Decommission();

  virtual void Tick() {}

  virtual void Tock();

  /// @brief Returns the nth parent region by traversing up the parent hierarchy
  /// @param layer The layer to find (1-indexed). Use -1 for the last/most
  /// distant region
  /// @return Pointer to the nth parent region, or nullptr if no parent region
  /// is found
  Region* GetParentRegion(int layer = 1);

  /// @brief Returns the nth parent institution by traversing up the parent
  /// hierarchy
  /// @param layer The layer to find (1-indexed). Use -1 for the last/most
  /// distant institution
  /// @return Pointer to the nth parent institution, or nullptr if no parent
  /// institution is found
  Institution* GetParentInstitution(int layer = 1);

  /// @brief Returns all parent regions by traversing up the hierarchy
  /// @return Vector of all parent regions, ordered from closest to farthest
  std::vector<Region*> GetAllParentRegions();

  /// @brief Returns all parent institutions by traversing up the hierarchy
  /// @return Vector of all parent institutions, ordered from closest to
  /// farthest
  std::vector<Institution*> GetAllParentInstitutions();

 protected:
  void InitFrom(Institution* m);
};

}  // namespace cyclus

#endif  // CYCLUS_SRC_INSTITUTION_H_
