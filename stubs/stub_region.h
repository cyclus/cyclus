#ifndef CYCLUS_STUBS_STUB_REGION_H_
#define CYCLUS_STUBS_STUB_REGION_H_

#include <string>

#include "cyclus.h"

namespace stubs {

/**
  @class StubRegion

  This Region is intended
  as a skeleton to guide the implementation of new Region agents.

  The StubRegion class inherits from the Region class and is
  dynamically loaded by the Agent class when requested.

  @section intro Introduction
  Place an introduction to the agent here.

  @section agentparams Agent Parameters
  Place a description of the required input parameters which define the
  agent implementation.

  @section optionalparams Optional Parameters
  Place a description of the optional input parameters to define the
  agent implementation.

  @section detailed Detailed Behavior
  Place a description of the detailed behavior of the agent. Consider
  describing the behavior at the tick and tock as well as the behavior
  upon sending and receiving materials and messages.
  */
class StubRegion : public cyclus::Region {
  /* --------------------
   * all REGIONMODEL classes have these members
   * --------------------
   */
 public:
  /**
    Constructor for StubRegion Class
    @param ctx the cyclus context for access to simulation-wide parameters
    */
  explicit StubRegion(cyclus::Context* ctx);

  /**
    every agent should be destructable
    */
  virtual ~StubRegion();

  /**
    Initialize db with input file info related to derived module class
    */
  virtual void InfileToDb(cyclus::QueryEngine* qe, cyclus::DbInit di);

  /**
    Initialize members for a cloned module.
    */
  virtual void InitFrom(StubRegion* m);

  /**
    A verbose printer for the StubRegion
    */
  virtual std::string str();

  /**
    Initializes a StubRegion object by copying the members of another.
    */
  virtual cyclus::Agent* Clone();

  /* -------------------- */


  /* --------------------
   * all REGIONMODEL classes have these members
   * --------------------
   */

  /* ------------------- */
};

}  // namespace stubs

#endif  // CYCLUS_STUBS_STUB_REGION_H_
