// StubCommmodel.h
#if !defined(_STUBCOMMMODEL_H)
#define _STUBCOMMMODEL_H

#include "model.h"
#include "communicator.h"

namespace cyclus {

/**
   The StubCommModel class is the abstract class/interface used by all 
   stub models. This StubCommModel is intended as a skeleton to guide 
   the implementation of new Models. 
    
   @section intro Introduction 
   The StubCommModel type plays no functional role in Cyclus. It is made 
   available as a skeletal example of a new Communicator model type, 
   showing the breadth and depth of available functional capabilities 
   that must or may be specified in order to create a new Communicator 
   model in Cyclus. In order to implement a new model in Cyclus 
   StubCommmodel.h and StubCommmodel.cc may be copied, renamed, and 
   altered as discussed in the <a 
 */
class StubCommModel : public Model, public Communicator {  
/* --------------------
 * all MODEL classes have these members
 * --------------------
 */
 public:
  /**
     Default constructor for StubCommModel Class 
   */
  StubCommModel();

  /**
     every model should be destructable 
   */
  virtual ~StubCommModel();

  /**
     every model should be able to print a verbose description 
   */
   virtual std::string str();

/* ------------------- */ 


/* --------------------
 * all COMMUNICATOR classes have these members
 * --------------------
 */
 public:
    /**
       No default STUBCOMMMODEL message receiver. 
     */
    virtual void ReceiveMessage(msg_ptr msg) = 0;

};

/* ------------------- */ 
} // namespace cyclus


#endif



