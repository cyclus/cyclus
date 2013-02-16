// EventManager.cpp

#include "EventManager.h"
#include "Event.h"

#define DEFAULT_DUMP_FREQ 10000

EventManager* EventManager::instance_ = 0;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
EventManager::EventManager() {
  dump_freq_ = DEFAULT_DUMP_FREQ;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  
EventManager* EventManager::Instance() {
  if (0 == instance_){
    instance_ = new EventManager();  
  }
  return instance_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
unsigned int EventManager::dump_freq(unsigned int size) {
  if (size == 0) {
    return dump_freq_;
  }
  dump_freq_ = size;
  return size;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
event_ptr EventManager::newEvent(Model* creator, std::string title) {
  event_ptr ev(new Event(this, creator, title));
  return ev;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool EventManager::isValidSchema(event_ptr ev) {
  if (schemas_.find(ev->name()) != schemas_.end()) {
    event_ptr primary = schemas_[ev->name()];
    if (! ev->schemaWithin(primary)) {
      return false;
    } 
  }
  return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void EventManager::addEvent(event_ptr ev) {
  if (! isValidSchema(ev)) {
    std::string msg;
    msg = "Name '" + ev->name() + "' with different schema already exists.";
    throw CycInvalidSchemaErr(msg);
  }

  schemas_[ev->title()] = ev;
  events_.push_back(ev);
  if (events_.size() >= dump_freq_) {
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
