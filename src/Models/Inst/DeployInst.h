// Deployinst.h
#if !defined(_DEPLOYINST_H)
#define _DEPLOYINST_H
#include <iostream>

#include "InstModel.h"

/**
 * The DeployInst class inherits from the InstModel class and is dynamically
 * loaded by the Model class when requested.
 * 
 * This model implements a simple institution model that deploys specific
 * facilities as defined explicitly in the input file.
 *
 */
class DeployInst : public InstModel  
{
/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

public:
  /**
   * Default constructor
   */
  DeployInst() {};

  /**
   * Destructor
   */
  ~DeployInst() {};
   
  // different ways to populate an object after creation
  /// initialize an object from XML input
  virtual void init(xmlNodePtr cur);

  /// initialize an object by copying another
  virtual void copy(DeployInst* src);

  virtual void print();

/* ------------------- */ 

/* --------------------
 * all COMMUNICATOR classes have these members
 * --------------------
 */
public:
  /// simply ignore incoming offers/requests.
  virtual void receiveOfferRequest(OfferRequest* msg) {};

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
  map<int,Model*> deployment_map;

/* ------------------- */ 


};

/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

extern "C" Model* construct() {
  return new DeployInst();
}

extern "C" void destruct(Model* p) {
  delete p;
}

/* -------------------- */

#endif
