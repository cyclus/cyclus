// Event.cpp

#include "Event.h"
#include "Timer.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
event_ptr Event::addVal(std::string field, boost::any val) {
  if (vals_.find(field) != vals_.end()) {
    throw CycDupEventField("Field '" + field + "' already exists in the event.");
  }
  vals_[field] = val;

  event_ptr ev(this);
  return ev;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
event_ptr Event::timestamp() {
  std::string field = "timestamp";
  boost::any val = TI->time();
  return addVal(field, val);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Event::record() {
  event_ptr ev(this);
  manager_->addEvent(ev);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Event::Event(Model* creator, std::string group) {
  creator_ = creator;
  group_ = group;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool Event::schemaWithin(event_ptr primary) {
  ValMap pvals = primary->vals_;

  for(ValMap::iterator it = vals_.begin(); it != vals_.end(); it++) {
    if (pvals.find(it->first) == pvals.end()) {
      return false;
    } else if (pvals[it->first]->type() != it->second->type()){
      return false;
    }
  }
  return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
std::string Event::group() {
  return group_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Model* Event::creator() {
  return creator_;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ValMap Event::vals() {
  return vals_;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
std::string Event::name() {
  Model* m = creator_;
  if (m != NULL) {
    return m->modelImpl() + "_" + m->ID() + "_" + e->group();
  }
  return e->group();
}

