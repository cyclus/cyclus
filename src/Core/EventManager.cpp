// EventManager.cpp

#include "EventManager.h"

#define DUMP_SIZE 100

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
event_ptr EventManager::newEvent(Model* creator, std::string group) {
  event_ptr ev(new Event(this, creator, group));
  return ev;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool EventManager::isValidSchema(event_ptr ev) {
  if (schemas_.find(creator_) != schemas_.end()) {
    if (schemas_[creator_].find(group_) != schemas.end()) {
      event_ptr primary = schemas_[creator_][group_];
      if (! ev->schemaWithin(primary)) {
        return false;
      } 
    }
  }
  return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void EventManager::addEvent(event_ptr ev) {
  if (! isValidSchema(ev)) {
    std::string msg;
    msg = "Group '" + group_ + "' with different schema already exists.";
    throw CycGroupDataMismatch(msg);
  }

  schemas_[creator_][group_] = ev;
  events_.push_back(ev);
  notifyBacks();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void EventManager::notifyBacks() {
  if (events_.size() < DUMP_SIZE) {
    return;
  }

  for(event_list::iterator it = events_.begin(); it != events_.end(); it++) {
    *it->notify(events_);
  }
  events_.clear();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void EventManager::registerBackend(EventBackend b) {
  backs_.push_back(b);
}
