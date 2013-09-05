// context.h
#ifndef CONTEXT_H_
#define CONTEXT_H_

#include <map>
#include <string>

#include "composition.h"
#include "event_manager.h"
#include "prototype.h"

namespace cyclus {

class EventManager;
class Event;
class MarketModel;
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
  void RegisterProto(std::string name, Prototype* p);

  /// Create a new model by cloning the named prototype. The returned model is
  /// not initialized as a simulation participant.
  template <class T>
  T* CreateModel(std::string proto_name) {
    if (protos_.count(proto_name) == 0) {
      throw KeyError("Invalid prototype name " + proto_name);
    }

    Prototype* p = protos_[proto_name];
    T* casted(NULL);
    casted = dynamic_cast<T*>(p->clone());
    if (casted == NULL) {
      throw CastError("Invalid prototype cast for prototype " + proto_name);
    }
    return casted;
  };

  /// Adds a composition recipe to a simulation-wide accessible list.
  void RegisterRecipe(std::string name, Composition::Ptr c);
  
  /// Retrieve a registered recipe.
  Composition::Ptr GetRecipe(std::string name);

  /// See Timer::RegisterTickListener documentation.
  void RegisterTicker(TimeAgent* ta);

  /// See Timer::RegisterResolveListener documentation.
  void RegisterResolver(MarketModel* mkt);

  /// Initializes the simulation time parameters. Should only be called once - NOT idempotent.
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

 private:
  std::map<std::string, Prototype*> protos_;
  std::map<std::string, Composition::Ptr> recipes_;

  Timer* ti_;
  EventManager* em_;
};

} // namespace cyclus

#endif
