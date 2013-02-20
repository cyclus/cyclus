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
Event::Event(EventManager* m, Model* creator, std::string title)
  : title_(title),
    creator_id_(-1),
    manager_(m) {
  if (creator != NULL) {
    creator_id_ = creator->ID();
    creator_impl_ = creator->modelImpl();
  }
}

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

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::string Event::name() {
  if (creator_id_ != -1) {
    std::stringstream ss;
    ss << creator_impl_ + "_" << creator_id_ << "_" + title_;
    return ss.str();
  }
  return title_;
}

