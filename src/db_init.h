#ifndef CYCLUS_SRC_DB_INIT_H_
#define CYCLUS_SRC_DB_INIT_H_

#include "datum.h"

namespace cyclus {

class Agent;

/// DbInit provides an interface for agents to record data to the output db that
/// automatically injects the agent's id and current timestep alongside all
/// recorded data.  The prefix 'AgentState' + [agent_impl] (e.g.
/// MyReactor) is also added to the datum title
class DbInit {
 public:
  DbInit(Agent* m);

  /// Using this constructor prevents the [agent_impl] from being injected into
  /// the title.
  DbInit(Agent* m, bool dummy);

  /// Returns a new datum to be used exactly as the Context::NewDatum method.
  /// Users must not add fields to the datum that are automatically injected:
  /// 'SimId', 'AgentId', and 'SimTime'.
  Datum* NewDatum(std::string title);

 private:
  bool full_prefix_;
  Agent* m_;
};

} // namespace cyclus
#endif

