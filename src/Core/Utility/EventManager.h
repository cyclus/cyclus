// EventManager.h
#if !defined(_EVENTMANAGER_H)
#define _EVENTMANAGER_H

#include "CycException.h"

#include <list>
#include <vector>
#include <string>
#include <map>
#include <boost/intrusive_ptr.hpp>
#include <boost/any.hpp>

// TODO: Move away from singleton pattern (that is why we kept EventManager constructor public)
#define EM EventManager::Instance()

class Event;
class EventManager;
class EventBackend;

typedef boost::intrusive_ptr<Event> event_ptr;
typedef std::vector<event_ptr> EventList;

/// default number of events to collect before flushing to backends.
static unsigned int const kDefaultDumpCount = 10000;

/*!
Indicates that a pre-recorded event of the same name (e.g. the same
table) has one or more inconsistent field-value type pairs.
*/
class CycInvalidSchemaErr: public CycException {
  public:
    CycInvalidSchemaErr(std::string msg) : CycException(msg) {};
};

/*!
Collects and manages output data generation for the cyclus core and agents
during a simulation.  By default, event managers are auto-initialized with a
unique uuid simulation id.

Example usage:

@code

EventManager* manager = new EventManager();
CustomBackend* myback = new CustomBackend();
manager->registerBackend(myback);
...
...
manager->newEvent("CapacityFactor");
       ->addVal("Name", aname)
       ->addVal("Capacity", cap)
       ->record();
...
...
manager->close();

@endcode
*/
class EventManager {
    friend class Event;

  private:
    bool isValidSchema(event_ptr ev);
    void notifyBackends();
    void addEvent(event_ptr ev);

    EventList events_;
    std::map<std::string, event_ptr> schemas_;
    std::list<EventBackend*> backs_;
    unsigned int dump_count_;
    std::string uuid_;
    std::string prefix_;

    /// A pointer to singleton EventManager.
    static EventManager* instance_;

  public:

    /// create a new event manager with default dump frequency.
    EventManager();
    // TODO: Move away from singleton pattern (that is why we kept the constructor public)

    /// Return the dump frequency, # events buffered between flushes to backends.
    unsigned int dump_count();

    /*!
    set the EventManager to flush its collected events to registered backends
    every [count] events. If count == 0 then events will be flushed immediately
    as they come.

    @param count # events to buffer before flushing to backends.
    */
    void set_dump_count(unsigned int count);

    /// returns the unique id associated with this cyclus simulation.
    std::string sim_id();

    /// adds a prefix to the auto-generated unique simulation id
    void setSimPrefix(std::string val);

    /*!
    Creates a new event namespaced under the specified title.

    @warning choose title carefully to not conflict with events from other
    agents. Also note that a static title (e.g. an unchanging string) will
    result in multiple instances of this agent storing event data together
    (e.g. the same table).
    */
    event_ptr newEvent(std::string title);

    /*!
    Registers b to receive event notifications for all events collected
    by the EventManager and to receive a close notification when there
    are no more events.

    @param b backend to receive events
    */
    void registerBackend(EventBackend* b);

    /// Flushes all events and closes all registered backends.
    void close();

    /// Returns a pointer to the global singleton EventManager
    static EventManager* Instance();

};

/*!
this allows files to use events without having to explicitly include
both EventManager.h and Event.h, while avoiding a circular include
dependency.
*/
#include "Event.h"

#endif
