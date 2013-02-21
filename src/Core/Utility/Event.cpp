// Event.cpp

#include "Event.h"
#include "Timer.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
event_ptr Event::addVal(std::string field, boost::any val) {
  if (vals_.find(field) != vals_.end()) {
    throw CycDupEventFieldErr("Field '" + field + "' already exists in the event.");
  }
  vals_[field] = val;

  event_ptr ev(this);
  return ev;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Event::record() {
  event_ptr ev(this);
  manager_->addEvent(ev);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Event::Event(EventManager* m, std::string title)
  : title_(title),
    manager_(m) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Event::~Event() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool Event::schemaWithin(event_ptr primary) {
  ValMap pvals = primary->vals_;

  for (ValMap::iterator it = vals_.begin(); it != vals_.end(); it++) {
    if (pvals.find(it->first) == pvals.end()) {
      return false;
    } else if (pvals[it->first].type() != it->second.type()) {
      return false;
    }
  }
  return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::string Event::title() {
  return title_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ValMap Event::vals() {
  return vals_;
}

