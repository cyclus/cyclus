// TimeAgent.h
#if !defined(_TIMEAGENT_H)
#define _TIMEAGENT_H

#include "Model.h"

using namespace std;

class TimeAgent : public Model {

public:

  /**
   * Each simulation agent is prompted to do its beginning-of-life
   * stuff.
   *
   */
  virtual void handlePreHistory() = 0;

  /**
   * Each simulation agent is prompted to do its beginning-of-time-step
   * stuff at the tick of the timer.
   *
   * @param time is the time to perform the tick
   */
  virtual void handleTick(int time) = 0;

  /**
   * Each simulation agent is prompted to its end-of-time-step
   * stuff on the tock of the timer.
   * 
   * @param time is the time to perform the tock
   */
  virtual void handleTock(int time) = 0;

};

#endif

