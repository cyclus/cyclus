// timer.h
#if !defined(_TIMER)
# define _TIMER

#include <utility>
#include <vector>
#include "boost/date_time/gregorian/gregorian.hpp"

#include "context.h"
#include "market_model.h"
#include "query_engine.h"
#include "time_agent.h"

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
  void RunSim();

  /**
     registers a sim. agent to receive time step notifications.

     @param agent agent that will receive time-step notifications
   */
  void RegisterTickListener(TimeAgent* agent);

  /**
     registers a sim. agent to receive (market) resolve notifications.

     @param agent agent that will receive resolve notifications
   */
  void RegisterResolveListener(MarketModel* agent);

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

  /**
     Returns the starting date of the simulation.

     @return the start date as a datetime object
   */
  boost::gregorian::date StartDate() {
    return start_date_;
  }

  /**
     Calculates the ending date of the simulation.

     @param startDate the starting date as specified in the input file
     @param simDur the simulation duration as specified

     @return the end date as a datetime object
   */
  boost::gregorian::date GetEndDate(boost::gregorian::date startDate, int simDur);

  /**
     Returns the ending date of the simulation.

     @return the end date as a datetime object
   */
  boost::gregorian::date endDate() {
    return end_date_;
  }

  /**
     Returns true if it is the ending date of the simulation

     @return whether it is the last day of the simulation
   */
  bool CheckEndDate() {
    return (date_ == end_date_);
  }

  /**
     Returns true if it is the ending month of the simulation

     @return whether it is the last day of the simulation
   */
  bool CheckEndMonth() {
    return (date_.month() == end_date_.month());
  }

  /**
     Given the current date, returns the last day of the current month

     @return the last date of the current month
   */
  int LastDayOfMonth();

  /**
     Returns the current date of the simulation.

     @return the current date as a datetime object
   */
  boost::gregorian::date date() {
    return date_;
  }

  /**
     Converts the given date into a GENIUS time.

     @param month the month corresponding to the date (Jan = 1, etc.)
     @param year the year corresponding to the date
     @return the GENIUS date
   */
  int ConvertDate(int month, int year);

  /**
     Converts the given GENIUS time into a (month, year) pair.
   */
  std::pair<int, int> ConvertDate(int time);

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
     The start date of the simulation
   */
  boost::gregorian::date start_date_;

  /**
     The end date of the simulation
   */
  boost::gregorian::date end_date_;

  /**
     The current date of the simulation
   */
  boost::gregorian::date date_;

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
  std::vector<TimeAgent*> tick_listeners_;

  /**
     Concrete models that desire to receive tick and tock notifications
   */
  std::vector<TimeAgent*> new_tickers_;

  /**
     Returns a string of all models listening to the tick
   */
  std::string ReportListeners();

  /**
     Concrete models that desire to receive resolve (markets)
   */
  std::vector<MarketModel*> resolve_listeners_;

  /**
     sends the resolve signal to all of the (market) models receiving
     resolve notifications.
   */
  void SendResolve();

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

  /**
     sends a notification to Tick listeners that a day has passed
   */
  void SendDailyTasks();

};
} // namespace cyclus
#endif

