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

class CondInjector: public QueryBackend {
 public:
  CondInjector(QueryBackend* b, std::vector<Cond> to_inject)
    : b_(b), to_inject_(to_inject) {};

  virtual QueryResult Query(std::string table, std::vector<Cond>* conds) {
    if (conds == NULL) {
      return b_->Query(table, &to_inject_);
    }

    std::vector<Cond> c = *conds;
    for (int i = 0; i < to_inject_.size(); ++i) {
      c.push_back(to_inject_[i]);
    }
    return b_->Query(table, &c);
  };

 private:
  QueryBackend* b_;
  std::vector<Cond> to_inject_;
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

 private:
  SimEngine* InitBase(QueryBackend* b, boost::uuids::uuid simid, int t);

  void LoadControlParams();
  void LoadRecipes();
  void LoadSolverInfo();
  void LoadBuildSched();
  void LoadDecomSched();
  void LoadPrototypes();
  void LoadInitialAgents();
  void LoadInventories();

  /// a map of loaded modules. all dynamically loaded modules are
  /// registered with this map when loaded.
  std::map< std::string, DynamicModule*> modules_;

  SimEngine* se_;
  boost::uuids::uuid simid_;
  QueryBackend* b_;
  int t_;
};

} // namespace cyclus

#endif

