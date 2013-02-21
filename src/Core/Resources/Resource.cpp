// Resource.cpp

#include "Resource.h"
#include "EventManager.h"

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

void Resource::setOriginalID(int id){
  originalID_ = id;
}

// -------------------------------------------------------------
// -------- output database related members  -------- 

void Resource::addToTable(){
  if (book_kept_) {
    return;
  }
  book_kept_ = true;

  // if we haven't recorded the resource type yet, do so
  if ( !this->is_resource_type_recorded() ){
    Resource::recordNewType();
    this->type_recorded();
  }
  
  EM->newEvent("Resources")
    ->addVal("ID", ID())
    ->addVal("Type", (int)type())
    ->addVal("OriginalQuantity", quantity())
    ->record();
}

void Resource::recordNewType(){
  EM->newEvent("ResourceTypes")
    ->addVal("Type", (int)type())
    ->addVal("Name", type_name())
    ->addVal("Units", units())
    ->record();
}
