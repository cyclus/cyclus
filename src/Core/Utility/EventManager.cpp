// EventManager.cpp

#include "EventManager.h"
#include "EventBackend.h"
#include "Event.h"
#include "Logger.h"

#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/lexical_cast.hpp>

EventManager* EventManager::instance_ = 0;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
EventManager::EventManager() : index_(0) {
  uuid_ = boost::uuids::random_generator()();
  set_dump_count(kDefaultDumpCount);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
EventManager::~EventManager() {
  for (int i = 0; i < events_.size(); ++i) {
    delete events_[i];
  }
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
boost::uuids::uuid EventManager::sim_id() {
  return uuid_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void EventManager::set_dump_count(unsigned int count) {
  for (int i = 0; i < events_.size(); ++i) {
    delete events_[i];
  }
  events_.clear();
  events_.reserve(count);
  for (int i = 0; i < count; ++i) {
    Event* ev = new Event(this, "");
    ev->addVal("SimID", uuid_);
    events_.push_back(ev);
  }
  dump_count_ = count;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Event* EventManager::newEvent(std::string title) {
  Event* ev = events_[index_];
  ev->title_ = title;
  ev->vals_.resize(1);
  index_++;
  return ev;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void EventManager::addEvent(Event* ev) {
  if (index_ >= events_.size()) {
    notifyBackends();
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void EventManager::notifyBackends() {
  index_ = 0;
  std::list<EventBackend*>::iterator it;
  for (it = backs_.begin(); it != backs_.end(); it++) {
    try {
      (*it)->notify(events_);
    } catch (CycException err) {
      CLOG(LEV_ERROR) << "Backend '" << (*it)->name()
                      << "' failed write with err: " << err.what();
    }
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void EventManager::registerBackend(EventBackend* b) {
  backs_.push_back(b);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void EventManager::close() {
  for (int i = index_; i < events_.size(); ++i) {
    delete events_[i];
  }
  events_.resize(index_);
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
