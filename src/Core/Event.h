// Event.h
#if !defined(_EVENT_H)
#define _EVENT_H

#include "IntrusiveBase.h"
#include "EventManager.h"

#include <list>
#include <string>
#include <boost/any.hpp>
#include <boost/intrusive_ptr.hpp>

class Event;
class EventManager;

typedef boost::intrusive_ptr<Event> event_ptr;
typedef std::map<std::string, boost::any> ValMap;

class Event: IntrusiveBase<Event> {
  public:
    friend class EventManager;

    // Add an arbitrary labeled value to the event.
    //
    // @param field a label or key for a value. Loosely analogous to a column label.
    //
    // @warning for the val argument - only types int, long, float, double, and
    // string are currently supported.
    event_ptr addVal(std::string field, boost::any val);

    // Add a timestamp value to this event of the current simulation time.
    // @return a pointer to this event (for method chaining).
    event_ptr timestamp();

    // Record this event to output.
    void record();

    std::string group();
    Model* creator();
    ValMap vals();
    std::string name();

    virtual ~Event();

  private:
    Event(EventManager* m, Model* creator, std::string group);
    bool schemaWithin(event_ptr primary);

    EventManager* manager_;
    std::string group_;
    Model* creator_;
    ValMap vals_;
};

#endif
