// StubModel.h
#if !defined(_STUBMODEL_H)
#define _STUBMODEL_H

#include "Model.h"

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
   to implement a new (non-communicator) model in Cyclus StubModel.h and 
   StubModel.cpp may be copied, renamed, and altered as discussed in the 
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
     Initalize members of StubModel and any other non-input
     related parameters - guarnatees all members that can be accessed are
     initialized (containers, etc.)

     This is called in the constructor, and is separate function in the case
     that there are multiple constructors. 
   */
  void init() {};

  /**
     every model needs a method to initialize from XML 
      
     @param cur is the pointer to the model's xml node 
   */
  virtual void init(xmlNodePtr cur);
  
  /**
     every model needs a method to copy one object to another 
      
     @param src is the StubModel to copy 
   */
  virtual void copy(StubModel* src) ;

  /**
     This drills down the dependency tree to initialize all relevant 
     parameters/containers.  
     Note that this function must be defined only in the specific model 
     in question and not in any inherited models preceding it. 
      
     @param src the pointer to the original (initialized ?) model to be 
   */
  virtual void copyFreshModel(Model* src){};

  /**
     every model should be able to print a verbose description 
   */
   virtual std::string str();

/* ------------------- */ 

};

#endif



