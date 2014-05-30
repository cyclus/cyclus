#ifndef CYCLUS_SRC_TOOLKIT_AGENT_MANAGED_H_
#define CYCLUS_SRC_TOOLKIT_AGENT_MANAGED_H_

#include "agent.h"

namespace cyclus {
namespace toolkit {

/// This is a mixin class that provides an interface to access the underlying
/// agent that manages it.
class AgentManaged {
 public:
  explicit AgentManaged(Agent* agent=NULL) : agent_(agent) {};
  inline Agent* agent() const {return agent_;}

 private:
  /// the agent managing this instance
  Agent* agent_;
};
  
} // namespace toolkit
} // namespace cyclus

#endif // CYCLUS_SRC_TOOLKIT_AGENT_MANAGED_H_
