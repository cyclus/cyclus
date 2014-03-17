// time_listener.h
#ifndef CYCLUS_TIME_LISTENER_H_
#define CYCLUS_TIME_LISTENER_H_

#include <string>

#include "context.h"
#include "model.h"

namespace cyclus {

/// The TimeListener class is an inheritable class for any Agent that
/// requires knowlege of ticks and tocks. The agent should register as a
/// TimeListener with its context from its Deploy method. For Example:
///
/// @begincode
///
/// MyFacilityAgent::Build(cyclus::Agent* parent) {
///   cyclus::FacilityAgent::Build(parent);
///   context()->RegisterTimeListener(this);
/// }
/// 
/// @endcode
class TimeListener {
 public:
  /// Simulation agents do their beginning-of-timestep activities in the Tick
  /// method.
  ///
  /// @param time is the current simulation timestep
  virtual void Tick(int time) = 0;

  /// Simulation agents do their end-of-timestep activities in the Tock
  /// method.
  ///
  /// @param time is the current simulation timestep
  virtual void Tock(int time) = 0;
};
} // namespace cyclus
#endif // ifndef CYCLUS_TIME_LISTENER_H_

