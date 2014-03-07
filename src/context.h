// context.h
#ifndef CYCLUS_SRC_CONTEXT_H_
#define CYCLUS_SRC_CONTEXT_H_

#include <map>
#include <set>
#include <string>

#include "composition.h"
#include "model.h"
#include "recorder.h"

namespace cyclus {

class Datum;
class ExchangeSolver;
class Recorder;
class Trader;
class Timer;
class TimeListener;
class SimInit;

class SimInfo {
 public:
  SimInfo(int dur, int y0, int m0, int decay_period, std::string handle = "")
    : duration(dur), y0(y0), m0(m0), decay_period(decay_period),
      branch_time(-1), handle(handle) {};

  SimInfo(int dur, int decay_period, boost::uuids::uuid parent_sim,
          int branch_time,
          std::string handle = "")
    : duration(dur), y0(-1), m0(-1), decay_period(decay_period),
      parent_sim(parent_sim),
      branch_time(branch_time), handle(handle) {};

  std::string handle;
  int duration;
  int y0;
  int m0;
  int decay_period;
  boost::uuids::uuid parent_sim;
  int branch_time;
};

/// A simulation context that provides access to necessary simulation-global
/// functions and state. All code that writes to the output database, needs to
/// know simulation time, creates/builds facilities, and/or uses loaded
/// composition recipes will need a context pointer. In general, all global
/// state should be accessed through a simulation context.
///
/// @warning the context takes ownership of and manages the lifetime/destruction
/// of all models constructed with it (including Cloned models). Models should
/// generally NEVER be allocated on the stack.
/// @warning the context takes ownership of the solver and will manage its
/// destruction.
class Context {
 public:
  friend class SimInit;
  friend class Model;

  /// Creates a new context working with the specified timer and datum manager.
  /// The timer does not have to be initialized (yet).
  Context(Timer* ti, Recorder* rec);

  /// Clean up resources including destructing the solver and all models the
  /// context is aware of.
  ~Context();

  /// See Recorder::sim_id documentation.
  boost::uuids::uuid sim_id();

  /// Adds a prototype to a simulation-wide accessible list.
  void AddPrototype(std::string name, Model* m);

  /// Registers an agent as a participant in resource exchanges. Agents should
  /// register from their Deploy method.
  inline void RegisterTrader(Trader* e) {
    traders_.insert(e);
  }

  /// Unregisters an agent as a participant in resource exchanges.
  inline void UnregisterTrader(Trader* e) {
    traders_.erase(e);
  }

  /// @return the current set of traders registered for resource exchange.
  inline const std::set<Trader*>& traders() const {
    return traders_;
  }

  /// Create a new model by cloning the named prototype. The returned model is
  /// not initialized as a simulation participant.
  ///
  /// @warning this method should generally NOT be used by agents.
  template <class T>
  T* CreateModel(std::string proto_name) {
    if (protos_.count(proto_name) == 0) {
      throw KeyError("Invalid prototype name " + proto_name);
    }

    Model* m = protos_[proto_name];
    T* casted(NULL);
    Model* clone = m->Clone();
    casted = dynamic_cast<T*>(clone);
    if (casted == NULL) {
      DelModel(clone);
      throw CastError("Invalid cast for prototype " + proto_name);
    }
    return casted;
  }

  /// Destructs and cleans up m (and it's children recursively).
  ///
  /// @warning this method should generally NOT be used by agents.
  void DelModel(Model* m);

  /// Schedules the named prototype to be built for the specified parent at
  /// timestep t. The default t=-1 results in the build being scheduled for the
  /// next build phase (i.e. the start of the next timestep).
  void SchedBuild(Model* parent, std::string proto_name, int t = -1);

  /// Schedules the given Model to be decommissioned at the specified timestep
  /// t. The default t=-1 results in the decommission being scheduled for the
  /// next decommission phase (i.e. the end of the current timestep).
  void SchedDecom(Model* m, int time = -1);

  /// Adds a composition recipe to a simulation-wide accessible list.
  /// Agents should NOT add their own recipes.
  void AddRecipe(std::string name, Composition::Ptr c);

  /// Retrieve a registered recipe.  This is intended for retrieving
  /// compositions loaded from an input file(s) at the start of a
  /// simulation and NOT for communicating compositions between facilities
  /// during the simulation.
  Composition::Ptr GetRecipe(std::string name);

  /// Registers an agent to receive tick/tock notifications every timestep.
  /// Agents should register from their Deploy method.
  void RegisterTimeListener(TimeListener* tl);

  /// Removes an agent from receiving tick/tock notifications.
  /// Agents should unregister from their Decommission method.
  void UnregisterTimeListener(TimeListener* tl);

  /// Initializes the simulation time parameters. Should only be called once -
  /// NOT idempotent.
  void InitSim(SimInfo si);

  /// Returns the current simulation timestep.
  int time();

  /// Returns the number of timesteps in the entire simulation.
  int sim_dur();

  /// See Recorder::NewDatum documentation.
  Datum* NewDatum(std::string title);

  /// Makes a snapshot of the simulation state to the output database.
  void Snapshot();

  /// @return the next transaction id
  inline int NextTransactionID() {
    return trans_id_++;
  }

  /// Returns the exchange solver associated with this context
  ExchangeSolver* solver() {
    return solver_;
  }

  /// sets the solver associated with this context
  void solver(ExchangeSolver* solver) {
    solver_ = solver;
  }

 private:
  std::map<std::string, Model*> protos_;
  std::map<std::string, Composition::Ptr> recipes_;
  std::set<Model*> model_list_;
  std::set<Trader*> traders_;

  Timer* ti_;
  ExchangeSolver* solver_;
  Recorder* rec_;
  int trans_id_;
};

}  // namespace cyclus

#endif  // CYCLUS_SRC_CONTEXT_H_



