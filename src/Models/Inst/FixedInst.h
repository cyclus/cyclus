// FixedInst.h
#if !defined(_FIXEDINST_H)
#define _FIXEDINST_H
#include <iostream>
#include <queue>

#include "InstModel.h"

/**
 * The FixedInst class inherits from the InstModel class and is dynamically
 * loaded by the Model class when requested.
 * 
 * This InstModel defines an institution that has an initial set of facilities
 * that continue operating forever.  No facilities are added and no facilities
 * are decommissioned.
 *
 */
class FixedInst : public InstModel  
{
/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

public:
  /**
   * Default constructor for the fixed inst
   */
  FixedInst() {};
    
  /**
   * Default destructor for the fixed inst
   */
  ~FixedInst() {};
  
  // different ways to populate an object after creation
  /// initialize an object from XML input
  virtual void init(xmlNodePtr cur);

  /// initialize an object from a map of pointers
  virtual void init(map<string, void*> member_var_map);

  /// initialize an object by copying another
  virtual void copy(FixedInst* src);

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
   * a print function to describe a fixedInst instantiation.
   */
  virtual void print();

/* ------------------- */ 

/* --------------------
 * all COMMUNICATOR classes have these members
 * --------------------
 */
public:

protected:
/* -------------------- */

/* --------------------
 * all INSTMODEL classes have these members
 * --------------------
 */


/* ------------------- */ 

/* --------------------
 * This INSTMODEL class has these members
 * --------------------
 */
  vector<Model*> facilities_;
  
  deque< pair< string, string > > fac_list_; 

/* ------------------- */ 


};

/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

extern "C" Model* construct() {
    return new FixedInst();
}

extern "C" void destruct(Model* p) {
    delete p;
}

/* -------------------- */

#endif
