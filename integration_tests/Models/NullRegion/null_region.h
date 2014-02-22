#ifndef CYCLUS_INTEGRATION_TESTS_MODELS_NULL_REGION_H_
#define CYCLUS_INTEGRATION_TESTS_MODELS_NULL_REGION_H_

#include <string>

#include "region_model.h"

namespace cyclus {

/**
  @class NullRegion

  This RegionModel is intended
  to be used as a basic region for Cyclus integration testing.

  @section intro Introduction
  Place an introduction to the model here.

  @section modelparams Model Parameters
  No modelparams are expected.

  @section optionalparams Optional Parameters
  No optionalparams are expected.

  @section detailed Detailed Behavior
  The model behavior should be similar to NullRegion of Cycamore.
  */

class NullRegion : public cyclus::RegionModel  {
  /* --------------------
   * all MODEL classes have these members
   * --------------------
   */
 public:
  /**
     The default constructor for the NullRegion
   */
  NullRegion(cyclus::Context* ctx);

  virtual cyclus::Model* Clone() {
    NullRegion* m = new NullRegion(context());
    m->InitFrom(this);
    return m;
  }

  /**
     The default destructor for the NullRegion
   */
  virtual ~NullRegion();

  /* ------------------- */

};

}  // namespace cyclus

#endif  // CYCLUS_INTEGRATION_TESTS_MODELS_NULL_REGION_H_
