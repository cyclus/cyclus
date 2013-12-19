// timer.h
#ifndef CYCLUS_TIMER_H_
#define CYCLUS_TIMER_H_

#include <utility>
#include <vector>

#include "context.h"
#include "query_engine.h"
#include "time_listener.h"

namespace cyclus {

/**
   @class Timer

   A timer to control a simulation with a one-month time step. The timer for a
   given simulation can be accessed via the simulation's Context.
 */
class Timer {
 public:
  Timer();

  /**
     Initialize this Timer by setting the specs for the simulation.

     @param ctx the simulation context
     @param dur the duration of this simulation, in months
     @param m0 the month the simulation starts: Jan. = 1, ..., Dec. = 12
     (default = 1)
     @param y0 the year the simulation starts (default = 2010)
     @param start the GENIUS time representing the first month of the
     simulation (default = 0)
     @param decay interval between decay calculations in months. <=0 if
     decay is off (default = 0)
     @param handle a user specified name for the simulation run
   */
  void Initialize(Context* ctx, int dur = 1, int m0 = 1, int y0 = 2010,
                  int start = 0, int decay = 0, std::string handle = "");

  /**
     reset all data (registered listeners, etc.) to empty or initial state
   */
  void Reset();

  /**
     Runs the simulation.
   */
  void RunSim(Context* ctx);

  /**
     registers a sim. agent to receive time step notifications.

     @param agent agent that will receive time-step notifications
   */
  void RegisterTickListener(TimeListener* agent);

  /**
     Returns the current time, in months since the simulation started.

     @return the current time
   */
  int time();

  /**
     Returns the the start time of the simulation
   */
  int start_time() {
    return start_time_;
  }

  /**
     Returns the duration of the simulation this Timer's timing.

     @return the duration, in months
   */
  int dur();

 private:
  /**
     logs relevant time-related data with the output system, including:
     the simulation start time and the simulation duration
   */
  void LogTimeData(Context* ctx, std::string handle);

  /**
     The current time, measured in months from when the simulation
     started.
   */
  int time_;

  /**
     The time at which the simulation started.
   */
  int start_time_;

  /**
     The duration of this simulation, in months.
   */
  int dur_;

  /// time steps between automated global material decay driving
  int decay_interval_;

  /**
     The number of the month (Jan = 1, etc.) corresponding to t = 0 for
     the scenario being run.
   */
  int month0_;

  /**
     The year corresponding to t = 0 for the scenario being run.
   */
  int year0_;

  /**
     Concrete models that desire to receive tick and tock notifications
   */
  std::vector<TimeListener*> tick_listeners_;

  /**
     Concrete models that desire to receive tick and tock notifications
   */
  std::vector<TimeListener*> new_tickers_;

  /**
     Returns a string of all models listening to the tick
   */
  std::string ReportListeners();

  /**
     sends the tick signal to all of the models receiving time
     notifications.
   */
  void SendTick();

  /**
     sends the tock signal to all of the models receiving time
     notifications.
   */
  void SendTock();
};

} // namespace cyclus

#endif // ifndef CYCLUS_TIMER_H_

