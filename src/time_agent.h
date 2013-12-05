// time_agent.h
#ifndef CYCLUS_TIMEAGENT_H_
#define CYCLUS_TIMEAGENT_H_

#include <string>

#include "context.h"
#include "model.h"

namespace cyclus {

/**
   The TimeAgent class is an inheritable class for any Agent
   that requires knowlege of ticks and tocks.
 */

class TimeAgent : virtual public Model {
 public:

  TimeAgent(Context* ctx) : Model(ctx) {};

  /**
     destructor
   */
  virtual ~TimeAgent() {};

  /**
     Each simulation agent is prompted to do its beginning-of-time-step
     stuff at the tick of the timer.

     @param time is the time to perform the tick
   */
  virtual void HandleTick(int time) = 0;

  /**
     Each simulation agent is prompted to its end-of-time-step
     stuff on the tock of the timer.

     @param time is the time to perform the tock
   */
  virtual void HandleTock(int time) = 0;

  /**
     Each simulation agent is prompted to do its daily tasks.

     @param time is current month since the start of the simulation
     @param day is the current day of that month
   */
  virtual void HandleDailyTasks(int time, int day) = 0;

};
} // namespace cyclus
#endif // ifndef CYCLUS_TIMEAGENT_H_

