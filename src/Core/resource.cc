// resource.cc

#include "resource.h"
#include "event_manager.h"

namespace cyclus {

// Resource IDs
int Resource::nextID_ = 0;

// -------------------------------------------------------------
Resource::Resource() {
  book_kept_ = false;
  ID_ = nextID_++;
  originalID_ = ID_;
  MLOG(LEV_DEBUG4) << "Resource ID=" << ID_ << ", ptr=" << this << " created.";
}

Resource::~Resource() {
  MLOG(LEV_DEBUG4) << "Resource ID=" << ID_ << ", ptr=" << this << " deleted.";
}

void Resource::SetOriginalID(int id) {
  originalID_ = id;
}

// -------------------------------------------------------------
// -------- output database related members  --------

void Resource::AddToTable() {
  if (book_kept_) {
    return;
  }
  book_kept_ = true;

  // if we haven't recorded the resource type yet, do so
  if (!this->is_resource_type_recorded()) {
    Resource::RecordNewType();
    this->type_recorded();
  }

  EM->NewEvent("Resources")
  ->AddVal("ID", ID())
  ->AddVal("Type", (int)type())
  ->AddVal("OriginalQuantity", quantity())
  ->Record();
}

void Resource::RecordNewType() {
  EM->NewEvent("ResourceTypes")
  ->AddVal("Type", (int)type())
  ->AddVal("Name", type_name())
  ->AddVal("Units", units())
  ->Record();
}
} // namespace cyclus
