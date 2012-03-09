// Deployinst.h
#if !defined(_DEPLOYINST_H)
#define _DEPLOYINST_H

#include <iostream>

#include "InstModel.h"

#include "Logger.h"

/**
   @class DeployInst
   @brief The DeployInst class inherits from the InstModel 
   class and is dynamically loaded by the Model class when requested.
   
   This model implements a simple institution model that deploys specific
   facilities as defined explicitly in the input file.
 */
class DeployInst : public InstModel {
/* --------------------
 * all MODEL classes have these members
 * --------------------
 */
 public:
  /**
   * @brief Default constructor
   */
  DeployInst() {};

  /**
   * @brief Destructor
   */
  virtual ~DeployInst() {};
   
  // different ways to populate an object after creation
  /**
   * @brief  initialize an object from XML input
   */
  virtual void init(xmlNodePtr cur);

  /**
   * @brief  initialize an object by copying another
   */
  virtual void copy(DeployInst* src);

  /**
   * @brief This drills down the dependency tree to initialize all relevant parameters/containers.
   *
   * Note that this function must be defined only in the specific model in question and not in any 
   * inherited models preceding it.
   *
   * @param src the pointer to the original (initialized ?) model to be copied
   */
  virtual void copyFreshModel(Model* src);

  /**
   * @brief prints information about this inst
   */
  virtual void print();

/* ------------------- */ 


/* --------------------
 * all COMMUNICATOR classes have these members
 * --------------------
 */
 public:
  /**
   * @brief  simply ignore incoming offers/requests.
   */
  virtual void receiveMessage(msg_ptr msg) {};

/* -------------------- */


/* --------------------
 * all INSTMODEL classes have these members
 * --------------------
 */
 public:
  /**
   * @brief tick handling function for this inst
   */
  virtual void handleTick(int time);
  
/* ------------------- */ 


/* --------------------
 * This INSTMODEL classes have these members
 * --------------------
 */
 protected:
  /**
   * @brief a map of deployed models
   */
  std::map<int,Model*> deployment_map_;

  /**
   * @brief a map of models to build
   */
  std::map<int,Model*> to_build_map_;
  
/* ------------------- */ 

};

#endif
