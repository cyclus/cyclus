// TimeAgent.h
#if !defined(_TIMEAGENT_H)
#define _TIMEAGENT_H

#include <string>

#include "Model.h"

/**
   @brief The TimeAgent class is an inheritable class for any Agent
   that requires knowlege of ticks and tocks.
 */

class TimeAgent : public Model {  
 public:
  /**
   * @brief Each simulation agent is prompted to do its beginning-of-life
   * stuff.
   */
  virtual void handlePreHistory() = 0;
  
  /**
   * @brief Each simulation agent is prompted to do its beginning-of-time-step
   * stuff at the tick of the timer.
   *
   * @param time is the time to perform the tick
   */
  virtual void handleTick(int time) = 0;

  /**
   * @brief Each simulation agent is prompted to its end-of-time-step
   * stuff on the tock of the timer.
   * 
   * @param time is the time to perform the tock
   */
  virtual void handleTock(int time) = 0;

  /**
   * @brief Each simulation agent is prompted to do its daily tasks.
   * 
   * @param time is current month since the start of the simulation
   * @param day is the current day of that month
   */
  virtual void handleDailyTasks(int time, int day) = 0;
  
};

#endif

