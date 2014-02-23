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

/// A simulation context that provides access to necessary simulation-global
/// functions and state. All code that writes to the output database, needs to
/// know simulation time, creates/deploys facilities, and/or uses loaded
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

  /// Registers an agent as a participant in resource exchanges
  inline void RegisterTrader(Trader* e) {
    traders_.insert(e);
  }

  /// Unregisters an agent as a participant in resource exchanges
  inline void UnregisterTrader(Trader* e) {
    traders_.erase(e);
  }

  /// Destructs and cleans up m (and it's children recursively).
  void DelModel(Model* m);

  /// @return the current set of facilities in the simulation
  inline const std::set<Trader*>& traders() const {
    return traders_;
  }

  /// Create a new model by cloning the named prototype. The returned model is
  /// not initialized as a simulation participant.
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

  /// Adds a composition recipe to a simulation-wide accessible list.
  /// Agents should NOT add their own recipes.
  void AddRecipe(std::string name, Composition::Ptr c);

  /// Retrieve a registered recipe.  This is intended for retrieving
  /// compositions loaded from an input file(s) at the start of a
  /// simulation and NOT for communicating compositions between facilities
  /// during the simulation.
  Composition::Ptr GetRecipe(std::string name);

  /// See Timer::RegisterTickListener documentation.
  void RegisterTimeListener(TimeListener* tl);

  /// Initializes the simulation time parameters. Should only be called once -
  /// NOT idempotent.
  void InitTime(int start, int duration, int decay, int m0 = 1, int y0 = 2010,
                std::string handle = "");

  /// Returns the current simulation timestep.
  int time();

  /// Returns the simulation starting timestep.
  int start_time();

  /// Returns the number of timesteps in the entire simulation.
  int sim_dur();

  /// See Recorder::NewDatum documentation.
  Datum* NewDatum(std::string title);

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
  std::set<Trader*> traders_;
  std::map<std::string, Composition::Ptr> recipes_;
  std::set<Model*> model_list_;

  Timer* ti_;
  ExchangeSolver* solver_;
  Recorder* rec_;
  int trans_id_;
};

}  // namespace cyclus

#endif  // CYCLUS_SRC_CONTEXT_H_
