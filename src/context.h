#ifndef CYCLUS_SRC_CONTEXT_H_
#define CYCLUS_SRC_CONTEXT_H_

#include <map>
#include <set>
#include <string>
#include <stdint.h>

#ifndef CYCPP
// The cyclus preprocessor cannot handle this file since there are two
// unmatch open braces '{' inside of strings that don't have cooresponding
// closed braces '}'
#include <boost/uuid/uuid_generators.hpp>
#endif

#include "composition.h"
#include "agent.h"
#include "greedy_solver.h"
#include "pyhooks.h"
#include "recorder.h"
#include "package.h"

const uint64_t kDefaultTimeStepDur = 2629846;

const uint64_t kDefaultSeed = 20160212;

const uint64_t kDefaultStride = 10000;

class SimInitTest;

namespace cyclus {

class Datum;
class ExchangeSolver;
class Recorder;
class Trader;
class Timer;
class TimeListener;
class SimInit;
class DynamicModule;
class RandomNumberGenerator;

/// Container for a static simulation-global parameters that both describe
/// the simulation and affect its behavior.
class SimInfo {
 public:
  /// @brief constructs a SimInfo instance with default variables
  /// @return a SimInfo instance
  SimInfo();

  /// @brief constructs a SimInfo instance using some default variables
  /// @param dur simulation duration in number of timesteps
  /// @param y0 start year for the simulation
  /// @param m0 start month for the simulation
  /// @param handle is this simulation's unique simulation handle
  /// @return a SimInfo instance
  SimInfo(int dur, int y0 = 2010, int m0 = 1,
          std::string handle = "");

  /// @brief constructs a SimInfo instance using no default variables
  /// @param dur simulation duration in number of timesteps
  /// @param y0 start year for the simulation
  /// @param m0 start month for the simulation
  /// @param handle is this simulation's unique simulation handle
  /// @param d the decay data member, "never" for no decay. "manual" otherwise
  /// @return a SimInfo instance
  SimInfo(int dur, int y0, int m0,
          std::string handle, std::string d);

  /// @brief constructs a SimInfo instance
  /// @param dur simulation duration in number of timesteps
  /// @param parent_sim the uuid of the parent simulation
  /// @param branch_time
  /// @param parent_type a string indicating the type of the parent simulation
  /// @param handle is this simulation's unique simulation handle
  /// @return a SimInfo instance
  SimInfo(int dur, boost::uuids::uuid parent_sim,
          int branch_time, std::string parent_type,
          std::string handle = "");

  /// user-defined label associated with a particular simulation
  std::string handle;

  /// "manual" if use of the decay function is allowed, "never" otherwise
  std::string decay;

  /// length of the simulation in timesteps (months)
  int duration;

  /// start year for the simulation (e.g. 1973);
  int y0;

  /// start month for the simulation: Jan = 1, ..., Dec = 12
  int m0;

  /// id for the parent simulation if any
  boost::uuids::uuid parent_sim;

  /// One of "init", "branch", "restart" indicating the relationship of this
  /// simulation to its parent simulation.
  std::string parent_type;

  /// timestep at which simulation branching occurs if any
  int branch_time;

  /// Duration in seconds of a single time step in the simulation.
  uint64_t dt;

  /// Epsilon in the simulation.
  double eps;

  /// Epsilon for resources in the simulation.
  double eps_rsrc;

  /// True if per-agent inventories should be explicitly queried/recorded
  /// every time step in a table (i.e. agent ID, Time, Nuclide, Quantity).
  bool explicit_inventory;

  /// True if per-agent inventories should be explicitly queried/recorded
  /// every time step in a table (i.e. agent ID, Time, Quantity,
  /// Composition-object and/or reference).
  bool explicit_inventory_compact;

  /// Seed for random number generator
  uint64_t seed;

  /// Stride length. Currently unused, but available for future development 
  /// that may wish to initiate multiple random number generators from the
  /// same seed, skipping forward in the sequence by the stride length times
  /// some parameter, such as the agent_id. 
  uint64_t stride;
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
  friend class Timer;

  /// Creates a new context working with the specified timer and datum manager.
  /// The timer does not have to be initialized (yet).
  Context(Timer* ti, Recorder* rec);

  /// Clean up resources including destructing the solver and all agents the
  /// context is aware of.
  ~Context();

  /// See Recorder::sim_id documentation.
  boost::uuids::uuid sim_id();

  /// Adds a prototype to a simulation-wide accessible list, a prototype **can
  /// not** be added more than once.
  /// @param name the prototype name
  /// @param m a pointer to the agent prototype
  /// @param overwrite, allow overwrites to the prototype listing, default: false
  /// @throws if overwrite is false and a prototype name has already been added
  /// @{
  void AddPrototype(std::string name, Agent* m);
  void AddPrototype(std::string name, Agent* m, bool overwrite);
  /// @}

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
      PyDelAgent(clone->id());
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
  virtual int time();

  /// Adds a package type to a simulation-wide accessible list.
  /// Agents should NOT add their own packages.
  void AddPackage(std::string name, double fill_min = 0,
                  double fill_max = std::numeric_limits<double>::max(),
                  std::string strategy = "first");

  /// Records package information. Should be used first on unpackaged, then
  /// to record user-declared packages
  void RecordPackage(Package::Ptr);

  /// Retrieve a registered package. 
  Package::Ptr GetPackage(std::string name);

  int random();

  /// Generates a random number on the range [0,1)]
  double random_01();

  /// Returns a random number from a uniform integer distribution.
  int random_uniform_int(int low, int high);

  /// Returns a random number from a uniform real distribution.
  double random_uniform_real(double low, double high);

  /// Returns a random number from a normal distribution.
  double random_normal_real(double mean, double std_dev, double low=0,
                            double high=std::numeric_limits<double>::max());

  /// Returns a random number from a lognormal distribution.
  int random_normal_int(double mean, double std_dev, int low=0,
                        int high=std::numeric_limits<int>::max());

  /// Returns the duration of a single time step in seconds.
  inline uint64_t dt() {return si_.dt;};

  /// Returns the seed for the random number generator.
  inline uint64_t seed() {return si_.seed;};

  /// Returns the stride for the random number generator.
  inline uint64_t stride() {return si_.stride;};

  /// Return static simulation info.
  inline SimInfo sim_info() const {
    return si_;
  }

  /// See Recorder::NewDatum documentation.
  Datum* NewDatum(std::string title);

  /// Schedules a snapshot of simulation state to output database to occur at
  /// the beginning of the next timestep.
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
    solver_->sim_ctx(this);
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

  /// contains archetype specs of all agents for which version have already
  /// been recorded in the db
  std::set<std::string> rec_ver_;

  std::map<std::string, Agent*> protos_;
  std::map<std::string, Composition::Ptr> recipes_;
  std::map<std::string, Package::Ptr> packages_;
  std::set<Agent*> agent_list_;
  std::set<Trader*> traders_;
  std::map<std::string, int> n_prototypes_;
  std::map<std::string, int> n_specs_;

  SimInfo si_;
  Timer* ti_;
  ExchangeSolver* solver_;
  Recorder* rec_;
  int trans_id_;
  RandomNumberGenerator* rng_;
};

}  // namespace cyclus

#endif  // CYCLUS_SRC_CONTEXT_H_
