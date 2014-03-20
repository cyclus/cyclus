#ifndef CYCLUS_INTEGRATION_TESTS_MODELS_NULL_INST_H_
#define CYCLUS_INTEGRATION_TESTS_MODELS_NULL_INST_H_

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
   * all MODEL classes have these members
   * --------------------
   */
 public:
  /**
     Default constructor
   */
  NullInst(cyclus::Context* ctx);

  /**
     Default destructor
   */
  virtual ~NullInst();

  virtual cyclus::Model* Clone() {
    NullInst* m = new NullInst(context());
    m->InitFrom(this);
    return m;
  }

  void InitFrom(NullInst* m) {
    cyclus::InstModel::InitFrom(m);
  }

  /* ------------------- */
  /* ------------------- */
};

}  // namespace cyclus

#endif  // CYCLUS_INTEGRATION_TESTS_MODELS_NULL_INST_H_
