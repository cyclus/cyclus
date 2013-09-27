// event_manager.cc

#include "event_manager.h"

#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/lexical_cast.hpp>

#include "event_backend.h"
#include "event.h"
#include "logger.h"

namespace cyclus {

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
    ev->AddVal("SimID", uuid_);
    events_.push_back(ev);
  }
  dump_count_ = count;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Event* EventManager::NewEvent(std::string title) {
  Event* ev = events_[index_];
  ev->title_ = title;
  ev->vals_.resize(1);
  index_++;
  return ev;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void EventManager::AddEvent(Event* ev) {
  if (index_ >= events_.size()) {
    NotifyBackends();
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void EventManager::NotifyBackends() {
  index_ = 0;
  std::list<EventBackend*>::iterator it;
  for (it = backs_.begin(); it != backs_.end(); it++) {
    (*it)->Notify(events_);
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void EventManager::RegisterBackend(EventBackend* b) {
  backs_.push_back(b);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void EventManager::close() {
  for (int i = index_; i < events_.size(); ++i) {
    delete events_[i];
  }
  events_.resize(index_);
  NotifyBackends();
  std::list<EventBackend*>::iterator it;
  for (it = backs_.begin(); it != backs_.end(); it++) {
    (*it)->Close();
  }
}
} // namespace cyclus

