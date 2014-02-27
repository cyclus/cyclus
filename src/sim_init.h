#ifndef CYCLUS_SIM_INIT_H_
#define CYCLUS_SIM_INIT_H_

#include <map>
#include <string>
#include <sstream>
#include <boost/shared_ptr.hpp>

#include "composition.h"
#include "dynamic_module.h"
#include "query_engine.h"
#include "xml_parser.h"

namespace cyclus {

class Context;

struct SimEngine {
  Context* ctx;
  Recorder* rec;
  Timer* ti;
}

/// a class that encapsulates the methods needed to load input to
/// a cyclus simulation from xml
class SimInit {
 public:
  SimInit();

  /// Initialize a simulation with data from b for the given simid.
  SimEngine* Init(QueryBackend* b, boost::uuids::uuid simid) {
    se_->rec = new Recorder(simid);
    b_ = b;
    simid_ = simid;
    t_ = 0;
    return InitBase();
  };

  /// Restarts a simulation from time t with data from b identified by simid.
  /// The newly configured simulation will run with a new simulation id.
  SimEngine* Restart(QueryBackend* b, boost::uuids::uuid simid, int t) {
    se_->rec = new Recorder();
    b_ = b;
    simid_ = simid;
    t_ = t;
    return InitBase()
  };

 private:
  SimEngine* InitBase();

  void LoadControlParams();
  void LoadRecipes();
  void LoadSolverInfo();
  void LoadPrototypes();
  void LoadInitialAgents();
  void LoadInventories();

  SimEngine* se_;
  boost::uuids::uuid simid_;
  QueryBackend* q_;
  int t_;
};

} // namespace cyclus

#endif

