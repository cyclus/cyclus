// StubTimeAgent.h
#if !defined(_STUBTIMEAGENT_H)
#define _STUBTIMEAGENT_H

#include "TimeAgent.h"

/**
   The StubTimeAgent class is the abstract class/interface used by all 
   stub models  
   This StubModel is intended as a skeleton to guide the implementation 
   of new TimeAgent Models. 
    
   @section intro Introduction 
   The StubTimeAgent type plays no functional role in Cyclus. It is made 
   available as a skeletal example of a new TimeAgent model type, 
   showing the breadth and depth of available functional capabilities 
   that must or may be specified in order to create a new TimeAgent 
   model in Cyclus. In order to implement a new model in Cyclus 
   StubTimeAgent.h and StubTimeAgent.cpp may be copied, renamed, and 
   altered as discussed in the <a 
 */
class StubTimeAgent : public TimeAgent {
/* --------------------
 * all MODEL classes have these members
 * --------------------
 */
 public:
  /**
     Default constructor for StubTimeAgent Class 
   */
  StubTimeAgent();

  /**
     every model should be destructable 
   */
  virtual ~StubTimeAgent();

  /**
     every model needs a method to initialize from XML 
      
     @param cur is the pointer to the model's xml node 
   */
  virtual void init(QueryEngine* qe,std::string cur_ns = "");

  /**
     every model should be able to print a verbose description 
   */
   virtual std::string str();

  /**
     Each simulation agent is prompted to do its beginning-of-life 
     stuff. 
      
   */
  virtual void handlePreHistory();

  /**
     Each simulation agent is prompted to do its beginning-of-time-step 
     stuff at the tick of the timer. 
      
     @param time is the time to perform the tick 
   */
  virtual void handleTick(int time);

  /**
     Each simulation agent is prompted to its end-of-time-step 
     stuff on the tock of the timer. 
      
     @param time is the time to perform the tock 
   */
  virtual void handleTock(int time);

/* ------------------- */ 

};

#endif

