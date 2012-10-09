// EventManager.cpp

#include "EventManager.h"
#include "Event.h"

#define DUMP_SIZE 500

EventManager* EventManager::instance_ = 0;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
EventManager* EventManager::Instance() {
  if (0 == instance_){
    instance_ = new EventManager();  
  }
  return instance_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
event_ptr EventManager::newEvent(Model* creator, std::string group) {
  event_ptr ev(new Event(this, creator, group));
  return ev;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool EventManager::isValidSchema(event_ptr ev) {
  if (schemas_.find(ev->creator()) != schemas_.end()) {
    std::map<std::string, event_ptr> subs = schemas_[ev->creator()];
    if (subs.find(ev->group()) != subs.end()) {
      event_ptr primary = subs[ev->group()];
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
    msg = "Group '" + ev->group() + "' with different schema already exists.";
    throw CycGroupDataMismatchErr(msg);
  }

  schemas_[ev->creator()][ev->group()] = ev;
  events_.push_back(ev);
  if (events_.size() >= DUMP_SIZE) {
    notifyBacks();
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void EventManager::notifyBacks() {
  std::list<EventBackend*>::iterator it;
  for(it = backs_.begin(); it != backs_.end(); it++) {
    try {
      (*it)->notify(events_);
    } catch (CycException err) {
      CLOG(LEV_ERROR) << "Backend '" << (*it)->name() << "' failed write with err: "
                      << err.what();
    }
  }
  events_.clear();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void EventManager::registerBackend(EventBackend* b) {
  backs_.push_back(b);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void EventManager::close() {
  notifyBacks();
  std::list<EventBackend*>::iterator it;
  for(it = backs_.begin(); it != backs_.end(); it++) {
    try {
      (*it)->close();
    } catch (CycException err) {
      CLOG(LEV_ERROR) << "Backend '" << (*it)->name() << "' failed to close with err: "
                      << err.what();
    }
  }
}
