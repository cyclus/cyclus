// Stubmodel.h
#if !defined(_STUBMODEL_H)
#define _STUBMODEL_H

#include "model.h"

namespace cyclus {

/**
   The StubModel class is the abstract class/interface used by all stub 
   models. 
    
   This StubModel is intended as a skeleton to guide the implementation 
   of new Models. 
    
   @section intro Introduction 
   The StubModel type plays no functional role in Cyclus. It is made 
   available as a skeletal example of a new model type, showing the 
   breadth and depth of available functional capabilities that must or 
   may be specified in order to create a new model in Cyclus. In order 
   to implement a new (non-communicator) model in Cyclus Stubmodel.h and 
   Stubmodel.cc may be copied, renamed, and altered as discussed in the 
   <a 
 */
class StubModel : public Model {
/* --------------------
 * all MODEL classes have these members
 * --------------------
 */
 public:
  /**
     Default constructor for StubModel Class 
   */
  StubModel();

  /**
     every model should be destructable 
   */
  virtual ~StubModel();
  
  /**
     every model should be able to print a verbose description 
   */
   virtual std::string str();

/* ------------------- */ 

};
} // namespace cyclus
#endif



