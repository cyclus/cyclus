// Timer.h
#if !defined(_TIMER)
# define _TIMER

#include <utility>
#include <vector>
#include "boost/date_time/gregorian/gregorian.hpp"

#include "TimeAgent.h"
#include "MarketModel.h"

class QueryEngine;

#define TI Timer::Instance()

/**
   @class Timer 
    
   A (singleton) timer to control a simulation with a one-month time 
 */
class Timer {
 private:
  /**
     A pointer to this Timer once it has been initialized. 
   */
  static Timer* instance_;

  /**
     The current time, measured in months from when the simulation 
     started. 
   */
  int time_;
    
  /**
     The time at which the simulation started. 
   */
  int time0_;
    
  /**
     The duration of this simulation, in months. 
   */
  int simDur_;

  /// time steps between automated global material decay driving
  int decay_interval_;
    
  /**
     The start date of the simulation 
   */
  boost::gregorian::date startDate_;
    
  /**
     The end date of the simulation 
   */
  boost::gregorian::date endDate_;
    
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
     Returns a string of all models listening to the tick 
   */
  std::string reportListeners();

  /**
     Concrete models that desire to receive resolve (markets) 
   */
  std::vector<MarketModel*> resolve_listeners_;

  /**
     sends the resolve signal to all of the (market) models receiving 
     resolve notifications. 
   */
  void sendResolve();

  /**
     sends the tick signal to all of the models receiving time 
     notifications. 
   */
  void sendTick();

  /**
     sends the tock signal to all of the models receiving time 
     notifications. 
   */
  void sendTock();
    
  /**
     sends a notification to Tick listeners that a day has passed 
   */
  void sendDailyTasks();

  /// reset all data (registered listeners, etc.) to empty or initial state
  void reset();

protected:
  /**
     Constructs a new Timer for this simulation. 
   */
  Timer();
  
public:
  /**
     Gives all simulation objects global access to the Timer by 
     returning a pointer to it. 
      
     @return a pointer to the Timer 
   */
  static Timer* Instance();

  /**
     Initialize this Timer by setting the specs for the simulation. 
      
     @param dur the duration of this simulation, in months 
     @param m0 the month the simulation starts: Jan. = 1, ..., Dec. = 12 
     (default = 1) 
     @param y0 the year the simulation starts (default = 2010) 
     @param start the GENIUS time representing the first month of the 
     simulation (default = 0) 
     @param decay interval between decay calculations in months. <=0 if 
     decay is off (default = 0) 
   */
  void initialize(int dur = 1, int m0 = 1, int y0 = 2010, int start = 0, int decay = 0);

  /**
     Runs the simulation. 
   */ 
  void runSim();

  /**
     registers a sim. agent to receive time step notifications. 
      
     @param agent agent that will receive time-step notifications 
   */
  void registerTickListener(TimeAgent* agent);

  /**
     registers a sim. agent to receive (market) resolve notifications. 
      
     @param agent agent that will receive resolve notifications 
   */
  void registerResolveListener(MarketModel* agent);

  /**
     Returns the current time, in months since the simulation started. 
      
     @return the current time 
   */
  int time();

  /**
     Returns the the start time of the simulation
   */
  int startTime() {return time0_;}

  /**
     Returns the the start time of the simulation
   */
  int finalTime() {return time0_+simDur_;}

  /**
     Returns the duration of the simulation this Timer's timing. 
      
     @return the duration, in months 
   */
  int simDur();

  /**
     Returns the starting date of the simulation. 
      
     @return the start date as a datetime object 
   */
  boost::gregorian::date startDate(){return startDate_;}

  /**
     Calculates the ending date of the simulation. 
      
     @param startDate the starting date as specified in the input file 
     @param simDur the simulation duration as specified 
      
     @return the end date as a datetime object 
   */
  boost::gregorian::date getEndDate(boost::gregorian::date startDate, int simDur);

  /**
     Returns the ending date of the simulation. 
      
     @return the end date as a datetime object 
   */
  boost::gregorian::date endDate(){return endDate_;}

  /**
     Returns true if it is the ending date of the simulation 
      
     @return whether it is the last day of the simulation 
   */
  bool checkEndDate() {return (date_ == endDate_);}

  /**
     Returns true if it is the ending month of the simulation 
      
     @return whether it is the last day of the simulation 
   */
  bool checkEndMonth() {return ( date_.month() == endDate_.month() );}
  
  /**
     Given the current date, returns the last day of the current month 
      
     @return the last date of the current month 
   */
  int lastDayOfMonth();

  /**
     Returns the current date of the simulation. 
      
     @return the current date as a datetime object 
   */
  boost::gregorian::date date(){return date_;}

  /**
     Converts the given date into a GENIUS time. 
      
     @param month the month corresponding to the date (Jan = 1, etc.) 
     @param year the year corresponding to the date 
     @return the GENIUS date 
   */
  int convertDate(int month, int year);

  /**
     Converts the given GENIUS time into a (month, year) pair. 
   */
  std::pair<int, int> convertDate(int time);

  /**
     Loads the information about the simulation timing 
     
     @param qe is a pointer to a general QueryEngine that can 
   */
  static void load_simulation(QueryEngine *qe);

  /**
     logs relevant time-related data with the output system, including:
     the simulation start time and the simulation duration
   */
  void logTimeData();
};
#endif

