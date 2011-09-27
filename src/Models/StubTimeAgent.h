// StubTimeAgent.h
#if !defined(_STUBTIMEAGENT_H)
#define _STUBTIMEAGENT_H
#include <string>

#include "TimeAgent.h"

using namespace std;

//-----------------------------------------------------------------------------
/*
 * The StubTimeAgent class is the abstract class/interface used by all stub models
 * 
 * This StubTimeAgent is intended as a skeleton to guide the implementation of new
 * Models.
 */
//-----------------------------------------------------------------------------
class StubTimeAgent : public TimeAgent {
/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

public:
  /**
   * Default constructor for StubTimeAgent Class
   */
  StubTimeAgent();

  /**
   * every model should be destructable
   */
  virtual ~StubTimeAgent();
    
  /**
   * every model needs a method to initialize from XML
   *
   * @param cur is the pointer to the model's xml node 
   */
  virtual void init(xmlNodePtr cur);
  
  /**
   * every model needs a method to copy one object to another
   *
   * @param src is the StubTimeAgent to copy
   */
  virtual void copy(StubTimeAgent* src) ;

  /**
   * This drills down the dependency tree to initialize all relevant parameters/containers.
   *
   * Note that this function must be defined only in the specific model in question and not in any 
   * inherited models preceding it.
   *
   * @param src the pointer to the original (initialized ?) model to be copied
   */
  virtual void copyFreshModel(Model* src){};

  /**
   * every model should be able to print a verbose description
   */
   virtual void print();

  /**
   * Each simulation agent is prompted to do its beginning-of-life
   * stuff.
   *
   */
  virtual void handlePreHistory();

  /**
   * Each simulation agent is prompted to do its beginning-of-time-step
   * stuff at the tick of the timer.
   *
   * @param time is the time to perform the tick
   */
  virtual void handleTick(int time);

  /**
   * Each simulation agent is prompted to its end-of-time-step
   * stuff on the tock of the timer.
   * 
   * @param time is the time to perform the tock
   */
  virtual void handleTock(int time);

};

#endif

