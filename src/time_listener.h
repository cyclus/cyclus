#ifndef CYCLUS_SRC_TIME_LISTENER_H_
#define CYCLUS_SRC_TIME_LISTENER_H_

#include <string>

#include "context.h"
#include "agent.h"

namespace cyclus {

/// The TimeListener class is an inheritable class for any Agent that
/// requires knowlege of ticks and tocks. The agent should register as a
/// TimeListener with its context from its Deploy method. For Example:
///
/// @code
///
/// MyFacility::Build(cyclus::Agent* parent) {
///   cyclus::Facility::Build(parent);
///   context()->RegisterTimeListener(this);
/// }
///
/// @endcode
class TimeListener: virtual public Ider {
 public:
  /// Simulation agents do their beginning-of-timestep activities in the Tick
  /// method.
  ///
  /// @param time is the current simulation timestep
  virtual void Tick() = 0;

  /// Simulation agents do their end-of-timestep activities in the Tock
  /// method.
  ///
  /// @param time is the current simulation timestep
  virtual void Tock() = 0;

  /// Simulation agents do their end-of-timestep decisions in the
  /// Decision method. The decision method allows for agents to
  /// make decision based on the operation of facilities during the
  /// tick and tock of the current timestep. Facility operations
  /// should not occur during this phase.
  ///
  /// @param time is the current simulation timestep
  virtual void Decision(){};

  virtual bool IsShim() = 0;
};

}  // namespace cyclus

#endif  // CYCLUS_SRC_TIME_LISTENER_H_
