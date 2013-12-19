// time_listener.h
#ifndef CYCLUS_TIME_LISTENER_H_
#define CYCLUS_TIME_LISTENER_H_

#include <string>

#include "context.h"
#include "model.h"

namespace cyclus {

/// The TimeListener class is an inheritable class for any Agent
/// that requires knowlege of ticks and tocks.
class TimeListener : virtual public Model {
 public:
  TimeListener(Context* ctx) : Model(ctx) {};

  virtual ~TimeListener() {};

  /// Each simulation agent is prompted to do its beginning-of-time-step
  /// stuff at the tick of the timer.
  ///
  /// @param time is the time to perform the tick
  virtual void Tick(int time) = 0;

  /// Each simulation agent is prompted to its end-of-time-step
  /// stuff on the tock of the timer.
  ///
  /// @param time is the time to perform the tock
  virtual void Tock(int time) = 0;
};
} // namespace cyclus
#endif // ifndef CYCLUS_TIME_LISTENER_H_

