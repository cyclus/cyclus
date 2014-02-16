#ifndef CYCLUS_INTEGRATION_TESTS_MODELS_SINK_FACILITY_H_
#define CYCLUS_INTEGRATION_TESTS_MODELS_SINK_FACILITY_H_

#include <string>

#include "context.h"
#include "facility_model.h"
#include "query_engine.h"

namespace cyclus {

/**
  @class SinkFacility
  Detailed description will be defined later.

  This FacilityModel is intended
  to be used in Cyclus integration tests as a basic sink facility.

  @section intro Introduction
  Place an introduction to the model here.

  @section modelparams Model Parameters
  Place a description of the required input parameters which define the
  model implementation.

  @section optionalparams Optional Parameters
  Place a description of the optional input parameters to define the
  model implementation.

  @section detailed Detailed Behavior
  Place a description of the detailed behavior of the model. Consider
  describing the behavior at the tick and tock as well as the behavior
  upon sending and receiving materials and messages.
  */
class SinkFacility : public cyclus::FacilityModel  {
  /* --------------------
   * all FACILITYMODEL classes have these members
   * --------------------
   */
 public:
  /**
    Constructor for SinkFacility Class
    @param ctx the cyclus context for access to simulation-wide parameters
    */
  explicit SinkFacility(cyclus::Context* ctx);

  /**
    every model should be destructable
    */
  virtual ~SinkFacility();

  /**
    Initialize members related to derived module class

    @param qe a pointer to a QueryEngine object containing initialization data
    */
  virtual void InitFrom(cyclus::QueryEngine* qe);

  /**
    Initialize members for a cloned module.
    */
  virtual void InitFrom(SinkFacility* m);

  /**
    A verbose printer for the SinkFacility
    */
  virtual std::string str();

  /**
    Initializes a SinkFacility object by copying the members of another.
    */
  virtual cyclus::Model* Clone();

  /**
    The handleTick function specific to the SinkFacility.

    @param time the time of the tick
    */
  virtual void Tick(int time);

  /**
    The handleTick function specific to the SinkFacility.

    @param time the time of the tock
    */
  virtual void Tock(int time);

  /* ------------------- */


  /* --------------------
   * _THIS_ FACILITYMODEL class has these members
   * --------------------
   */

  /* ------------------- */
};

}  // namespace cyclus

#endif  // CYCLUS_INTEGRATION_TESTS_MODELS_SINK_FACILITY_H_
