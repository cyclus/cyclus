// FixedInst.h
#if !defined(_BUILDINST_H)
#define _BUILDINST_H
#include <iostream>
#include "Logger.h"

#include "InstModel.h"

/**
   The BuildInst class inherits from the InstModel class and is dynamically
   loaded by the Model class when requested.
   
   This InstModel defines an institution that has an initial set of facilities
   that continue operating forever.  No facilities are added and no facilities
   are decommissioned.
   
 */
class BuildInst : public InstModel  
{
/* --------------------
 * all MODEL classes have these members
 * --------------------
 */

public:
  /**
     Default constructor for the fixed inst
   */
  BuildInst() {};
    
  /**
     Default destructor for the fixed inst
   */
  virtual ~BuildInst() {};
   
  // different ways to populate an object after creation
  /// initialize an object from XML input
  virtual void init(xmlNodePtr cur)  { InstModel::init(cur); };

  /// initialize an object by copying another
  virtual void copy(BuildInst* src);

  /**
     This drills down the dependency tree to initialize all relevant parameters/containers.
     
     Note that this function must be defined only in the specific model in question and not in any 
     inherited models preceding it.
     
     @param src the pointer to the original (initialized ?) model to be copied
   */
  virtual void copyFreshModel(Model* src);

  /**
     a print function to describe a fixedInst instantiation.
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
 * This INSTMODEL classes have these members
 * --------------------
 */

protected:

  vector<Model*> facilities_;

/* ------------------- */ 


/* --------------------
 * the BuildInst class have these members
 * --------------------
 */

 public:
  /**
     The build function.
   */
  bool pleaseBuild(Model* fac);


/* --------------------
   output directory info
 * --------------------
 */
 public:
  /**
     The getter function for the this inst model output dir
  */
  static std::string outputDir(){ 
    return InstModel::outputDir().append(outputDir_);}
  
 private:
  /**
     Every specific inst model writes to the output database
     location: InstModel::OutputDir_ + /inst_model_name
  */
  static std::string outputDir_;


/* ------------------- */ 

};

#endif
