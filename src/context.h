// context.h
#ifndef CYCLUS_SRC_CONTEXT_H_
#define CYCLUS_SRC_CONTEXT_H_

#include <map>
#include <set>
#include <string>

#ifndef CYCPP
// The cyclus preprocessor cannot handle this file since there are two 
// unmatch open braces '{' inside of strings that don't have cooresponding
// closed braces '}'
#include <boost/uuid/uuid_generators.hpp>
#endif

#include "composition.h"
#include "agent.h"
#include "greedy_solver.h"
#include "recorder.h"

class SimInitTest;

namespace cyclus {

class Datum;
class ExchangeSolver;
class Recorder;
class Trader;
class Timer;
class TimeListener;
class SimInit;

/// Container for a static simulation-global parameters that both describe
/// the simulation and affect its behavior.
class SimInfo {
 public:
  SimInfo()
    : duration(0), y0(0), m0(0), decay_period(0),
      branch_time(-1), parent_type("init") {};

  SimInfo(int dur, int y0 = 2010, int m0 = 1, int decay_period = -1, std::string handle = "")
    : duration(dur), y0(y0), m0(m0), decay_period(decay_period),
      branch_time(-1), handle(handle), parent_type("init"), parent_sim(boost::uuids::nil_uuid()) {};

  SimInfo(int dur, int decay_period, boost::uuids::uuid parent_sim,
          int branch_time, std::string parent_type,
          std::string handle = "")
    : duration(dur), y0(-1), m0(-1), decay_period(decay_period),
      parent_sim(parent_sim), parent_type(parent_type),
      branch_time(branch_time), handle(handle) {};

  /// user-defined label associated with a particular simulation
  std::string handle;

  /// length of the simulation in timesteps (months)
  int duration;

  /// start year for the simulation (e.g. 1973);
  int y0;

  /// start month for the simulation: Jan = 1, ..., Dec = 12
  int m0;

  /// interval between decay calculations in timesteps (months)
  int decay_period;

  /// id for the parent simulation if any
  boost::uuids::uuid parent_sim;

  /// One of "init", "branch", "restart" indicating the relationship of this
  /// simulation to its parent simulation.
  std::string parent_type;

  /// timestep at which simulation branching occurs if any
  int branch_time;
};

/// A simulation context provides access to necessary simulation-global
/// functions and state. All code that writes to the output database, needs to
/// know simulation time, creates/builds facilities, and/or uses loaded
/// composition recipes will need a context pointer. In general, all global
/// state should be accessed through a simulation context.
///
/// @warning the context takes ownership of and manages the lifetime/destruction
/// of all agents constructed with it (including Cloned agents). Agents should
/// generally NEVER be allocated on the stack.
/// @warning the context takes ownership of the solver and will manage its
/// destruction.
class Context {
 public:
  friend class ::SimInitTest;
  friend class SimInit;
  friend class Agent;

  /// Creates a new context working with the specified timer and datum manager.
  /// The timer does not have to be initialized (yet).
  Context(Timer* ti, Recorder* rec);

  /// Clean up resources including destructing the solver and all agents the
  /// context is aware of.
  ~Context();

  /// See Recorder::sim_id documentation.
  boost::uuids::uuid sim_id();

  /// Adds a prototype to a simulation-wide accessible list.
  void AddPrototype(std::string name, Agent* m);

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

  /// Create a new agent by cloning the named prototype. The returned agent is
  /// not initialized as a simulation participant.
  ///
  /// @warning this method should generally NOT be used by agents.
  template <class T>
  T* CreateAgent(std::string proto_name) {
    if (protos_.count(proto_name) == 0) {
      throw KeyError("Invalid prototype name " + proto_name);
    }

    Agent* m = protos_[proto_name];
    T* casted(NULL);
    Agent* clone = m->Clone();
    casted = dynamic_cast<T*>(clone);
    if (casted == NULL) {
      DelAgent(clone);
      throw CastError("Invalid cast for prototype " + proto_name);
    }
    return casted;
  }

  /// Destructs and cleans up m (and it's children recursively).
  ///
  /// @warning this method should generally NOT be used by agents.
  void DelAgent(Agent* m);

  /// Schedules the named prototype to be built for the specified parent at
  /// timestep t. The default t=-1 results in the build being scheduled for the
  /// next build phase (i.e. the start of the next timestep).
  void SchedBuild(Agent* parent, std::string proto_name, int t = -1);

  /// Schedules the given Agent to be decommissioned at the specified timestep
  /// t. The default t=-1 results in the decommission being scheduled for the
  /// next decommission phase (i.e. the end of the current timestep).
  void SchedDecom(Agent* m, int time = -1);

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

  /// Return static simulation info.
  inline SimInfo sim_info() const {return si_;};

  /// See Recorder::NewDatum documentation.
  Datum* NewDatum(std::string title);

  /// Schedules a snapshot of simulation state to output database to occur at
  /// the end of the current timestep.
  void Snapshot();

  /// Schedules the simulation to be terminated at the end of this timestep.
  void KillSim();

  /// @return the next transaction id
  inline int NextTransactionID() {
    return trans_id_++;
  }

  /// Returns the exchange solver associated with this context
  ExchangeSolver* solver() {
    if (solver_ == NULL) {
      solver_ = new GreedySolver(false, NULL);
    }
    return solver_;
  }

  /// sets the solver associated with this context
  void solver(ExchangeSolver* solver) {
    solver_ = solver;
  }

  /// @return the number of agents of a given prototype currently in the
  /// simulation
  inline int n_prototypes(std::string type) {
    return n_prototypes_[type];
  }

  /// @return the number of agents of a given implementation currently in the
  /// simulation
  inline int n_specs(std::string impl) {
    return n_specs_[impl];
  }
  
 private:
  /// Registers an agent as a participant in the simulation. 
  inline void RegisterAgent(Agent* a) {
    n_prototypes_[a->prototype()]++;
    n_specs_[a->spec()]++;
  }

  /// Unregisters an agent as a participant in the simulation.
  inline void UnregisterAgent(Agent* a) {
    n_prototypes_[a->prototype()]--;
    n_specs_[a->spec()]--;
  }

  std::map<std::string, Agent*> protos_;
  std::map<std::string, Composition::Ptr> recipes_;
  std::set<Agent*> agent_list_;
  std::set<Trader*> traders_;
  std::map<std::string, int> n_prototypes_;
  std::map<std::string, int> n_specs_;
  
  SimInfo si_;
  Timer* ti_;
  ExchangeSolver* solver_;
  Recorder* rec_;
  int trans_id_;
};

}  // namespace cyclus

#endif  // CYCLUS_SRC_CONTEXT_H_



