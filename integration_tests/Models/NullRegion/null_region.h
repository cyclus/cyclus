#ifndef CYCLUS_INTEGRATION_TESTS_MODELS_NULL_REGION_H_
#define CYCLUS_INTEGRATION_TESTS_MODELS_NULL_REGION_H_

#include <string>

#include "context.h"
#include "region_model.h"
#include "query_engine.h"

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
class NullRegion : public cyclus::RegionModel {
  /* --------------------
   * all REGIONMODEL classes have these members
   * --------------------
   */
 public:
  /**
    Constructor for NullRegion Class
    @param ctx the cyclus context for access to simulation-wide parameters
    */
  explicit NullRegion(cyclus::Context* ctx);

  /**
    every model should be destructable
    */
  virtual ~NullRegion();

  /**
    Initialize members related to derived module class

    @param qe a pointer to a QueryEngine object containing initialization data
    */
  virtual void InitFrom(cyclus::QueryEngine* qe);

  /**
    Initialize members for a cloned module.
    */
  virtual void InitFrom(NullRegion* m);

  /**
    A verbose printer for the NullRegion
    */
  virtual std::string str();

  /**
    Initializes a NullRegion object by copying the members of another.
    */
  virtual cyclus::Model* Clone();

  /* -------------------- */


  /* --------------------
   * all REGIONMODEL classes have these members
   * --------------------
   */

  /* ------------------- */
};

}  // namespace cyclus

#endif  // CYCLUS_INTEGRATION_TESTS_MODELS_NULL_REGION_H_
