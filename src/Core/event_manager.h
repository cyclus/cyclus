// event_manager.h
#ifndef EVENTMANAGER_H_
#define EVENTMANAGER_H_

#include <list>
#include <string>
#include <vector>
#include <boost/uuid/uuid.hpp>

#include "error.h"

namespace cyclus {

class Event;
class EventManager;
class EventBackend;

typedef std::vector<Event*> EventList;

/// default number of events to collect before flushing to backends.
static unsigned int const kDefaultDumpCount = 10000;

/// Collects and manages output data generation for the cyclus core and agents
/// during a simulation.  By default, event managers are auto-initialized with a
/// unique uuid simulation id.
///
/// Example usage:
///
/// @code
///
/// EventManager* manager = new EventManager();
/// CustomBackend* myback = new CustomBackend();
/// manager->RegisterBackend(myback);
/// ...
/// ...
/// manager->NewEvent("CapacityFactor");
///        ->AddVal("Name", aname)
///        ->AddVal("Capacity", cap)
///        ->Record();
/// ...
/// ...
/// manager->close();
///
/// @endcode
class EventManager {
  friend class Event;

 public:

  /// create a new event manager with default dump frequency.
  EventManager();

  ~EventManager();

  /// Return the dump frequency, # events buffered between flushes to backends.
  unsigned int dump_count();

  /// set the EventManager to flush its collected events to registered backends
  /// every [count] events. If count == 0 then events will be flushed immediately
  /// as they come.

  /// @param count # events to buffer before flushing to backends.
  void set_dump_count(unsigned int count);

  /// returns the unique id associated with this cyclus simulation.
  boost::uuids::uuid sim_id();

  /// Creates a new event namespaced under the specified title.

  /// @warning choose title carefully to not conflict with events from other
  /// agents. Also note that a static title (e.g. an unchanging string) will
  /// result in multiple instances of this agent storing event data together
  /// (e.g. the same table).
  Event* NewEvent(std::string title);

  /// Registers b to receive event notifications for all events collected
  /// by the EventManager and to receive a close notification when there
  /// are no more events.

  /// @param b backend to receive events
  void RegisterBackend(EventBackend* b);

  /// Flushes all events and closes all registered backends.
  void close();

 private:
  void NotifyBackends();
  void AddEvent(Event* ev);

  EventList events_;
  int index_;
  std::list<EventBackend*> backs_;
  unsigned int dump_count_;
  boost::uuids::uuid uuid_;
};
} // namespace cyclus

// this allows files to use events without having to explicitly include
// both event_manager.h and event.h, while avoiding a circular include
// dependency.
#include "event.h"

#endif
