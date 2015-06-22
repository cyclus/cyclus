#ifndef CYCLUS_SRC_SIM_INIT_H_
#define CYCLUS_SRC_SIM_INIT_H_

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

  /// Initialize a simulation with data from b for simulation id in r. SimInit
  /// does not take ownership of the recorder or backend. the configured
  /// context's recorder is set to r.
  void Init(Recorder* r, QueryableBackend* b);

  /// EXPERIMENTAL (might not work properly). Restarts a simulation from time t
  /// with data from b identified by simid.  The newly configured simulation
  /// will run with a new simulation id.
  void Restart(QueryableBackend* b, boost::uuids::uuid sim_id, int t);

  /// NOT IMPLEMENTED. Initializes a simulation branched from prev_sim_id at
  /// time t with diverging state described in new_sim_id.
  ///
  /// TODO(rwcarlsen): implement
  void Branch(QueryableBackend* b, boost::uuids::uuid prev_sim_id, int t,
              boost::uuids::uuid new_sim_id);

  /// Records a snapshot of the current state of the simulation being managed by
  /// ctx into the simulation's output database.
  static void Snapshot(Context* ctx);

  /// Records a snapshot of the agent's current internal state into the
  /// simulation's output database.  Note that this should generally not be
  /// called directly.
  static void SnapAgent(Agent* m);

  /// Returns the initialized context. Note that either Init, Restart, or Branch
  /// must be called first.
  Context* context() { return ctx_; }

  /// Returns the initialized recorder with registered backends. Note that
  /// either Init, Restart, or Branch must be called first.
  Recorder* recorder() { return rec_; }

  /// Returns the initialized timer. Note that either Init, Restart, or Branch
  /// must be called first.
  Timer* timer() { return &ti_; }

  /// Convenience function for reconstructing an untracked material object with
  /// the given resource state id from a database backend b.  Particularly
  /// useful for running mock simulations/tests.
  static Material::Ptr BuildMaterial(QueryableBackend* b, int resid);

  /// Convenience function for reconstructing an untracked product object with
  /// the given resource state id from a database backend b.  Particularly
  /// useful for running mock simulations/tests.
  static Product::Ptr BuildProduct(QueryableBackend* b, int resid);

 private:
  void InitBase(QueryableBackend* b, boost::uuids::uuid simid, int t);

  void LoadInfo();
  void LoadRecipes();
  void LoadSolverInfo();
  void LoadPrototypes();
  void LoadInitialAgents();
  void LoadInventories();
  void LoadBuildSched();
  void LoadDecomSched();
  void LoadNextIds();

  void* LoadPreconditioner(std::string name);
  ExchangeSolver* LoadGreedySolver(bool exclusive, std::set<std::string> tables);
  ExchangeSolver* LoadOptimizeSolver(bool exclusive, std::set<std::string> tables);
  static Resource::Ptr LoadResource(Context* ctx, QueryableBackend* b, int resid);
  static Material::Ptr LoadMaterial(Context* ctx, QueryableBackend* b, int resid);
  static Product::Ptr LoadProduct(Context* ctx, QueryableBackend* b, int resid);
  static Composition::Ptr LoadComposition(QueryableBackend* b, int stateid);

  // std::map<AgentId, Agent*>
  std::map<int, Agent*> agents_;

  Context* ctx_;
  Recorder* rec_;
  Timer ti_;
  boost::uuids::uuid simid_;
  SimInfo si_;
  QueryableBackend* b_;
  int t_;
};

}  // namespace cyclus

#endif  // CYCLUS_SRC_SIM_INIT_H_
