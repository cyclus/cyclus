// StubCommModel.h
#if !defined(_STUBCOMMMODEL_H)
#define _STUBCOMMMODEL_H
#include <string>


#include "Model.h"
#include "Communicator.h"

using namespace std;




//-----------------------------------------------------------------------------
/*
 * The StubCommModel class is the abstract class/interface used by all stub models
 * 
 * This StubCommModel is intended as a skeleton to guide the implementation of new
 * Models.
 */
//-----------------------------------------------------------------------------
class StubCommModel : public Model, public Communicator
{
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
   * Note that this function must be defined only in the specfici model in question and not in any 
   * inherited models preceding it.
   *
   * @param src the pointer to the original (initialized ?) model to be copied
   */
  virtual void copyFreshModel(Model* src){};

  /**
   * every model should be able to print a verbose description
   */
   virtual void print();

protected: 
    /// Stores the next available stub ID
    static int nextID;

/* ------------------- */ 


/* --------------------
 * all COMMUNICATOR classes have these members
 * --------------------
 */
public:
    /// No default STUBCOMMMODEL message receiver.
    virtual void receiveMessage(Message* msg) = 0;

protected:


/* ------------------- */ 


/* --------------------
 * all STUBCOMMMODEL classes have these members
 * --------------------
 */

/* ------------------- */ 
    
};

#endif



