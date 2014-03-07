#ifndef CYCLUS_SRC_DB_INIT_H_
#define CYCLUS_SRC_DB_INIT_H_

#include "datum.h"

namespace cyclus {

class Model;

/// DbInit provides an interface for agents to record data to the output db that
/// automatically injects the agent's id and current timestep alongside all
/// recorded data.  The prefix 'AgentState_' is also added to the datum title.
class DbInit {
 public:
  /// Returns a new datum to be used exactly as the Context::NewDatum method.
  /// Users must not add fields to the datum that are automatically injected:
  /// 'SimId', 'AgentId', and 'Time'.
  Datum* NewDatum(Model* m, std::string title);
};

} // namespace cyclus
#endif

