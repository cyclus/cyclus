#ifndef CYCLUS_SIM_INIT_H_
#define CYCLUS_SIM_INIT_H_

#include <boost/uuid/uuid_io.hpp>

#include "query_backend.h"
#include "context.h"
#include "timer.h"
#include "recorder.h"

namespace cyclus {

class Context;

struct SimEngine {
  Context* ctx;
  Recorder* rec;
  Timer* ti;
};

/// a class that encapsulates the methods needed to load input to
/// a cyclus simulation from xml
class SimInit {
 public:
  SimInit();

  /// Initialize a simulation with data from b for the given simid.
  SimEngine* Init(QueryBackend* b, boost::uuids::uuid simid);

  /// Restarts a simulation from time t with data from b identified by simid.
  /// The newly configured simulation will run with a new simulation id.
  SimEngine* Restart(QueryBackend* b, boost::uuids::uuid simid, int t);

  static void Snapshot(Context* ctx);

 private:
  SimEngine* InitBase(QueryBackend* b, boost::uuids::uuid simid, int t);

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

  SimEngine* se_;
  boost::uuids::uuid simid_;
  QueryBackend* b_;
  int t_;
};

} // namespace cyclus

#endif

