#ifndef CYCLUS_SRC_STATE_WRANGLER_H_
#define CYCLUS_SRC_STATE_WRANGLER_H_

#include <string>

#include "db_init.h"
#include "query_engine.h"
#include "query_backend.h"

namespace cyclus {

/// An abjstract interface that must be implemented by all simulation agents and
/// all agent member variables that have unexported internal state.
///
/// These methods all do inter-related things.  Notably, the InfileToDb,
/// InitFrom, and Snapshot methods must all write/read to/from the same database
/// tables (and table schemas). The InfileToDb method reads data from the
/// QueryEngine that is first validated with the rng schema returned by the
/// schema method.
class StateWrangler {
 public:
  /// Return a newly created/allocated object that is an exact copy of this.
  /// Subclasses are expected to override the return type with their own
  /// concrete type.
  virtual StateWrangler* Clone() = 0;

  /// Translates info for the object from input file information to the database by reading
  /// parameters from the passed QueryEngine and recording data via the DbInit
  /// variable.  The simulation and agent id's are automatically injected in all
  /// data transfered through di.
  ///
  /// Agent parameters in the QueryEngine are scoped in the
  /// "model/[model-class-name]" path. The model's class-name can be retrieved
  /// from the model_impl method. The superclass InitFrom expects the QueryEngine
  /// passed to it to be scoped identically - do NOT pass a changed-scope
  /// QueryEngine to the superclass.
  ///
  /// @warning this method MUST NOT modify the object's state.
  virtual void InfileToDb(QueryEngine* qe, DbInit di) = 0;

  /// Intializes an agent's internal state from an output database. Appropriate
  /// simulation id, agent id, and time filters are automatically included in
  /// all queries.
  virtual void InitFrom(QueryBackend* b) = 0;

  /// Snapshots agent-internal state to the output db via di. This method MUST
  /// call the superclass' Snapshot method before doing any work. The simulation
  /// and agent id's in addition to the snapshot time are automatically included
  /// in all information transfered through di.
  ///
  /// @warning because a 'Time' field is automatically injected, that label
  /// cannot be used for any other fields.
  ///
  /// @warning This method MUST NOT modify the agent's state.
  virtual void Snapshot(DbInit di) = 0;

  /// Returns an object's xml rng schema for initializing from input files.
  virtual std::string schema() = 0;
};
} // namespace cyclus

#endif
