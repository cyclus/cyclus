#ifndef NULL_INST_H_
#define NULL_INST_H_

#include <string>

#include "context.h"
#include "inst_model.h"
#include "query_engine.h"

namespace cyclus {

/**
  @class NullInst

  This InstModel is intended
  as a basic institution for Cyclus integration testing.

  @section intro Introduction
  This institution performs the most basics functions just enough for
  integration testing.

  @section modelparams Model Parameters
  Model parameters are not expected for this model.

  @section optionalparams Optional Parameters
  Optional parameters are not expected for this model.

  @section detailed Detailed Behavior
  The behaviour of this model should be similar to the null institution
  model implemented in cycamore.
  */
class NullInst : public cyclus::InstModel {
  /* --------------------
   * all INSTMODEL classes have these members
   * --------------------
   */
 public:
  /**
    Constructor for NullInst Class
    @param ctx the cyclus context for access to simulation-wide parameters
    */
  NullInst(cyclus::Context* ctx);

  /**
    every model should be destructable
    */
  virtual ~NullInst();

  /**
    Initializes the module member data from the data in a QueryEngine object

    @param qe is a QueryEngine object that contains intialization data
    */
  virtual void InitFrom(cyclus::QueryEngine* qe);

  /**
    Initialize members for a cloned module.
    */
  virtual void InitFrom(NullInst* m);

  /**
    Initializes a NullInst object by copying the members of another.
    */
  virtual cyclus::Model* Clone();

  /**
    every model should be able to print a verbose description
    */
  virtual std::string str();

  /* ------------------- */


  /* --------------------
   * _THIS_ INSTMODEL class has these members
   * --------------------
   */

  /* ------------------- */

};

} // namespace cyclus

#endif // NULL_INST_H_
