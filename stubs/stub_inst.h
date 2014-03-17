#ifndef CYCLUS_STUBS_STUB_INST_H_
#define CYCLUS_STUBS_STUB_INST_H_

#include <string>

#include "cyclus.h"

namespace stubs {

/**
  @class StubInst

  This Institution is intended
  as a skeleton to guide the implementation of new Institution agents.

  The StubInst class inherits from the Institution class and is
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
class StubInst : public cyclus::Institution {
  /* --------------------
   * all INSTMODEL classes have these members
   * --------------------
   */
 public:
  /**
    Constructor for StubInst Class
    @param ctx the cyclus context for access to simulation-wide parameters
    */
  explicit  StubInst(cyclus::Context* ctx);

  /**
    every agent should be destructable
    */
  virtual ~StubInst();

  /**
    Initialize db with input file info related to derived module class
    */
  virtual void InfileToDb(cyclus::InfileTree* qe, cyclus::DbInit di);

  /**
    Initialize members for a cloned module.
    */
  virtual void InitFrom(StubInst* m);

  /**
    Initializes a StubInst object by copying the members of another.
    */
  virtual cyclus::Agent* Clone();

  /**
    every agent should be able to print a verbose description
    */
  virtual std::string str();

  /* ------------------- */


  /* --------------------
   * _THIS_ INSTMODEL class has these members
   * --------------------
   */

  /* ------------------- */
};

}  // namespace stubs

#endif  // CYCLUS_STUBS_STUB_INST_H_
