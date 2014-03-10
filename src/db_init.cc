
#include "db_init.h"

#include "context.h"
#include "model.h"

namespace cyclus {

DbInit::DbInit(Model* m) : m_(m) {}

Datum* DbInit::NewDatum(std::string title) {
  Datum* d = m_->context()->NewDatum("AgentState" + m_->model_impl() + title);
  d->AddVal("AgentId", m_->id());
  d->AddVal("Time", m_->context()->time());
  return d;
}

} // namespace cyclus
