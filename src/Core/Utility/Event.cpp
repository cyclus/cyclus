// Event.cpp

#include "Event.h"
#include "Timer.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
event_ptr Event::addVal(const char* field, boost::any val) {
  vals_.push_back(std::pair<const char*, boost::any>(field, val));
  return event_ptr(this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Event::record() {
  manager_->addEvent(event_ptr(this));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Event::Event(EventManager* m, std::string title)
  : title_(title),
    manager_(m),
    count_(0) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Event::~Event() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::string Event::title() {
  return title_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const Event::Vals& Event::vals() {
  return vals_;
}

