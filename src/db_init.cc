
#include "db_init.h"

#include "context.h"
#include "model.h"

namespace cyclus {

Datum* DbInit::NewDatum(Model* m, std::string title) {
  Datum* d = m->context()->NewDatum(title);
  d->AddVal("AgentId", m->id());
  d->AddVal("Time", m->context()->time());
  return d;
}

} // namespace cyclus
