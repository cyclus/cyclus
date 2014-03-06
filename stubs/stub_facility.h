#ifndef CYCLUS_STUBS_STUB_FACILITY_H_
#define CYCLUS_STUBS_STUB_FACILITY_H_

#include <string>

#include "cyclus.h"

namespace stubs {

/**
  @class StubFacility

  This FacilityModel is intended
  as a skeleton to guide the implementation of new FacilityModel
  models.
  The StubFacility class inherits from the FacilityModel class and is
  dynamically loaded by the Model class when requested.

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
class StubFacility : public cyclus::FacilityModel  {
  /* --------------------
   * all FACILITYMODEL classes have these members
   * --------------------
   */
 public:
  /**
    Constructor for StubFacility Class
    @param ctx the cyclus context for access to simulation-wide parameters
    */
  explicit  StubFacility(cyclus::Context* ctx);

  /**
    every model should be destructable
    */
  virtual ~StubFacility();

  /**
    Initialize db with input file info related to derived module class
    */
  virtual void InfileToDb(cyclus::QueryEngine* qe, cyclus::DbInit di);

  /**
    Initialize members for a cloned module.
    */
  virtual void InitFrom(StubFacility* m);

  /**
    A verbose printer for the StubFacility
    */
  virtual std::string str();

  /**
    Initializes a StubFacility object by copying the members of another.
    */
  virtual cyclus::Model* Clone();

  /**
    The handleTick function specific to the StubFacility.

    @param time the time of the tick
    */
  virtual void Tick(int time);

  /**
    The handleTick function specific to the StubFacility.

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

}  // namespace stubs

#endif  // CYCLUS_STUBS_STUB_FACILITY_H_
