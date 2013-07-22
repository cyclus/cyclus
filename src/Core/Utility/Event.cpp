// Event.cpp

#include "Event.h"
#include "Timer.h"

typedef boost::singleton_pool<Event, sizeof(Event)> EventPool;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Event* Event::addVal(const char* field, boost::spirit::hold_any val) {
  vals_.push_back(std::pair<const char*, boost::spirit::hold_any>(field, val));
  return this;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Event::record() {
  manager_->addEvent(this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Event::Event(EventManager* m, std::string title)
  : title_(title),
    manager_(m) {
  // The (vect) size to reserve is chosen to be just bigger than most/all cyclus
  // core tables.  This prevents extra reallocations in the underlying
  // vector as vals are added to the event.
  vals_.reserve(10);
}

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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void* Event::operator new(size_t size) {
  if (size != sizeof(Event)) 
    return ::operator new(size);

  while(true) {
    void* ptr = EventPool::malloc();
    if (ptr != NULL) return ptr;

    std::new_handler globalNewHandler = std::set_new_handler(0);
    std::set_new_handler(globalNewHandler);

    if(globalNewHandler) globalNewHandler();
    else throw std::bad_alloc();
  }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Event::operator delete(void * rawMemory) throw() {
  if(rawMemory == 0) return; 
  EventPool::free(rawMemory);
}
