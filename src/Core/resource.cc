
#include "resource.h"

#include "event_manager.h"

namespace cyclus {

int Resource::nextid_ = 1;

void Resource::BumpId() {
  id_ = nextid_;
  nextid_++;

  EM->NewEvent("Resources")
  ->AddVal("ID", id_)
  ->AddVal("Type", type())
  ->AddVal("Quantity", quantity())
  ->AddVal("units", units())
  ->AddVal("StateId", state_id())
  ->Record();
}

} // namespace cyclus
