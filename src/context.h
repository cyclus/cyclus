// context.h
#ifndef CONTEXT_H_
#define CONTEXT_H_

#include <map>
#include <string>
#include <set>

#include "composition.h"
#include "event_manager.h"
#include "model.h"

namespace cyclus {

class EventManager;
class Event;
class Trader;
class Timer;
class TimeAgent;

/// A simulation context that provides access to necessary simulation-global
/// functions and state. All code that writes to the output database, needs to
/// know simulation time, creates/deploys facilities, and/or uses loaded
/// composition recipes will need a context pointer. In general, all global
/// state should be accessed through a simulation context.
class Context {
 public:
  /// Creates a new context working with the specified timer and event manager.
  /// The timer does not have to be initialized (yet).
  Context(Timer* ti, EventManager* em);

  /// See EventManager::sim_id documentation.
  boost::uuids::uuid sim_id();

  /// Adds a prototype to a simulation-wide accessible list.
  void AddPrototype(std::string name, Model* m);

  /// Adds a model to a simulation-wide accessible list.
  inline void AddModel(Model* m) { model_list_.push_back(m); }

  /**
     returns a model given the prototype's name

     @param name name of the prototype as defined in the input file
     @throws KeyError if name is not found
   */
  Model* GetModelByName(std::string name);
  
  /// Access the simulation-wide model list.
  inline const std::vector<Model*>& model_list() const { return model_list_; }
  inline std::vector<Model*>& model_list() { return model_list_; }

  /// Registers an agent as a participant in resource exchanges
  inline void RegisterTrader(Trader* e) {
    traders_.insert(e);
  }

  /// Unregisters an agent as a participant in resource exchanges
  inline void UnregisterTrader(Trader* e) {
    traders_.erase(e);
  }

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
      delete clone;
      throw CastError("Invalid cast for prototype " + proto_name);
    }
    return casted;
  };

  /// Adds a composition recipe to a simulation-wide accessible list.
  /// Agents should NOT add their own recipes.
  void AddRecipe(std::string name, Composition::Ptr c);
  
  /// Retrieve a registered recipe.  This is intended for retrieving
  /// compositions loaded from an input file(s) at the start of a
  /// simulation and NOT for communicating compositions between facilities
  /// during the simulation.
  Composition::Ptr GetRecipe(std::string name);

  /// See Timer::RegisterTickListener documentation.
  void RegisterTicker(TimeAgent* ta);

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

  /// See EventManager::NewEvent documentation.
  Event* NewEvent(std::string title);

  /// @return the next transaction id
  inline int NextTransactionID() { return trans_id_++; }
      
 private:
  std::map<std::string, Model*> protos_;
  std::set<Trader*> traders_;
  std::map<std::string, Composition::Ptr> recipes_;
  std::vector<Model*> model_list_;
  
  Timer* ti_;
  EventManager* em_;
  int trans_id_;
};

} // namespace cyclus

#endif
