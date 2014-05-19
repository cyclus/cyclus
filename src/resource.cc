
#include "resource.h"

namespace cyclus {

int Resource::nextstate_id_ = 1;
int Resource::nextobj_id_ = 1;

void Resource::BumpStateId() {
  state_id_ = nextstate_id_;
  nextstate_id_++;
}

} // namespace cyclus
