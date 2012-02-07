// StubCommModel.h
#if !defined(_STUBCOMMMODEL_H)
#define _STUBCOMMMODEL_H

#include "Model.h"
#include "Communicator.h"

/**
   @brief The StubCommModel class is the abstract class/interface used by all 
   stub models. This StubCommModel is intended as a skeleton to guide the 
   implementation of new Models.

   @section intro Introduction
   The StubCommModel type plays no functional role in Cyclus. It is made 
   available as a skeletal example of a new Communicator model type, showing 
   the breadth and depth of available functional capabilities that must or 
   may be specified in order to create a new Communicator model in Cyclus. In 
   order to implement a new model in Cyclus StubCommModel.h and 
   StubCommModel.cpp may be copied, renamed, and altered as discussed in the 
   <a href="http://code.google.com/p/cyclus/wiki/GuidelinesForImplementingNewModels">GuidelinesForImplementingNewModels</a>.
 */

//-----------------------------------------------------------------------------
/*
 * 
 */
//-----------------------------------------------------------------------------
class StubCommModel : public Model, public Communicator {
/* --------------------
 * all MODEL classes have these members
 * --------------------
 */
  

public:
  /**
   * Default constructor for StubCommModel Class
   */
  StubCommModel();

  /**
   * every model should be destructable
   */
  virtual ~StubCommModel();
    
  /**
   * every model needs a method to initialize from XML
   *
   * @param cur is the pointer to the model's xml node 
   */
  virtual void init(xmlNodePtr cur);
  
  /**
   * every model needs a method to copy one object to another
   *
   * @param src is the StubCommModel to copy
   */
  virtual void copy(StubCommModel* src) ;

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

/* --------------------
 * all COMMUNICATOR classes have these members
 * --------------------
 */
public:
    /// No default STUBCOMMMODEL message receiver.
    virtual void receiveMessage(msg_ptr msg) = 0;

};

#endif



