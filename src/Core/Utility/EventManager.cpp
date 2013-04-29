// EventManager.cpp

#include "EventManager.h"
#include "Event.h"

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid_io.hpp>

EventManager* EventManager::instance_ = 0;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
EventManager::EventManager() : dump_count_(kDefaultDumpCount) {
  boost::uuids::uuid uuid = boost::uuids::random_generator()();
  sim_id_ = boost::lexical_cast<std::string>(uuid);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
EventManager* EventManager::Instance() {
  if (0 == instance_) {
    instance_ = new EventManager();
  }
  return instance_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
unsigned int EventManager::dump_count() {
  return dump_count_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::string EventManager::sim_id() {
  return sim_id_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void EventManager::set_dump_count(unsigned int count) {
  dump_count_ = count;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
event_ptr EventManager::newEvent( std::string title) {
  event_ptr ev(new Event(this, title));
  return ev;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool EventManager::isValidSchema(event_ptr ev) {
  if (schemas_.find(ev->title()) != schemas_.end()) {
    event_ptr primary = schemas_[ev->title()];
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
    msg = "Name '" + ev->title() + "' with different schema already exists.";
    throw CycInvalidSchemaErr(msg);
  }

  if (schemas_.find(ev->title()) == schemas_.end()) {
    schemas_[ev->title()] = ev;
  }

  events_.push_back(ev);
  if (events_.size() >= dump_count_) {
    notifyBackends();
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void EventManager::notifyBackends() {
  std::list<EventBackend*>::iterator it;
  for (it = backs_.begin(); it != backs_.end(); it++) {
    try {
      (*it)->notify(events_);
    } catch (CycException err) {
      CLOG(LEV_ERROR) << "Backend '" << (*it)->name()
                      << "' failed write with err: " << err.what();
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
  notifyBackends();
  std::list<EventBackend*>::iterator it;
  for (it = backs_.begin(); it != backs_.end(); it++) {
    try {
      (*it)->close();
    } catch (CycException err) {
      CLOG(LEV_ERROR) << "Backend '" << (*it)->name()
                      << "' failed to close with err: " << err.what();
    }
  }
}
