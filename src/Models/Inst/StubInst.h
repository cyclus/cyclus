// StubInst.h
#if !defined(_STUBINST_H)
#define _STUBINST_H
#include <iostream>
#include "Logger.h"

#include "InstModel.h"

/**
 * The StubInst class inherits from the InstModel class and is dynamically
 * loaded by the Model class when requested.
 * 
 * This model will do nothing. This InstModel is intended as a skeleton to guide
 * the implementation of new InstModel models. 
 *
 */
class StubInst : public InstModel  
{

/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

public:
  /**
   * Default constructor for StubInst Class
   */
  StubInst();

  /**
   * every model should be destructable
   */
  ~StubInst();
    
  /**
   * every model needs a method to initialize from XML
   *
   * @param cur is the pointer to the model's xml node 
   */
  virtual void init(xmlNodePtr cur);
  
  /**
   * every model needs a method to copy one object to another
   *
   * @param src is the StubStub to copy
   */
  virtual void copy(StubInst* src) ;

  /**
   * This drills down the dependency tree to initialize all relevant parameters/containers.
   *
   * Note that this function must be defined only in the specific model in question and not in any 
   * inherited models preceding it.
   *
   * @param src the pointer to the original (initialized ?) model to be copied
   */
  virtual void copyFreshModel(Model* src);

  /**
   * every model should be able to print a verbose description
   */
   virtual void print();

/* ------------------- */ 

/* --------------------
 * all COMMUNICATOR classes have these members
 * --------------------
 */
public:

    /**
     * The StubInst should ignore incoming messages
     */
    virtual void receiveMessage(Message* msg);

protected:


/* -------------------- */

/* --------------------
 * all INSTMODEL classes have these members
 * --------------------
 */

/* ------------------- */ 

/* --------------------
 * This INSTMODEL classes have these members
 * --------------------
 */

protected:

/* ------------------- */ 


};

/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

extern "C" Model* construct() {
    return new StubInst();
}

extern "C" void destruct(Model* p) {
    delete p;
}

/* -------------------- */

#endif
