#ifndef CYCLUS_SIM_INIT_H_
#define CYCLUS_SIM_INIT_H_

#include <boost/uuid/uuid_io.hpp>

#include "query_backend.h"
#include "context.h"
#include "timer.h"
#include "recorder.h"

namespace cyclus {

class Context;

/// Handles initialization of a simulation from the output database. After
/// calling Init, Restart, or Branch, the initialized Context, Timer, and
/// Recorder can be retrieved.
///
/// @warning the Init, Restart, and Branch methods are NOT idempotent. Only one
/// simulation should ever be initialized per SimInit object.
///
/// @warning the SimInit class manages the memory of the initialized Context,
/// Timer, and Recorder.
class SimInit {
 public:
  SimInit();

  ~SimInit();

  /// Initialize a simulation with data from b for the given simulation id.
  void Init(QueryBackend* b, boost::uuids::uuid sim_id);

  /// Restarts a simulation from time t with data from b identified by simid.
  /// The newly configured simulation will run with a new simulation id.
  void Restart(QueryBackend* b, boost::uuids::uuid sim_id, int t);

  /// NOT_IMPLEMENTED. Initializes a simulation branched from prev_sim_id at
  /// time t with diverging state described in new_sim_id.
  ///
  /// TODO: implement
  void Branch(QueryBackend* b, boost::uuids::uuid prev_sim_id, int t,
                    boost::uuids::uuid new_sim_id);

  /// Records a snapshot of the current state of the simulation being managed by
  /// ctx into the simulations output database.
  static void Snapshot(Context* ctx);

  /// Returns the initialized context. Note that either Init, Restart, or Branch
  /// must be called first.
  Context* context() {return ctx_;};
  
  /// Returns the initialized recorder with registered backends. Note that
  /// either Init, Restart, or Branch must be called first.
  Recorder* recorder() {return rec_;};

  /// Returns the initialized timer. Note that either Init, Restart, or Branch
  /// must be called first.
  Timer* timer() {return &ti_;};

 private:
  void InitBase(QueryBackend* b, boost::uuids::uuid simid, int t);

  void LoadInfo();
  void LoadRecipes();
  void LoadSolverInfo();
  void LoadPrototypes();
  void LoadInitialAgents();
  void LoadInventories();
  void LoadBuildSched();
  void LoadDecomSched();
  void LoadNextIds();

  Resource::Ptr LoadResource(int resid);
  Resource::Ptr LoadMaterial(int resid);
  Resource::Ptr LoadGenericResource(int resid);
  Composition::Ptr LoadComposition(int stateid);

  static void SnapAgent(Model* m);

  // std::map<AgentId, Model*>
  std::map<int, Model*> agents_;

  Context* ctx_;
  Recorder* rec_;
  Timer ti_;
  boost::uuids::uuid simid_;
  QueryBackend* b_;
  int t_;
};

} // namespace cyclus

#endif


