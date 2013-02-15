// Event.h
#if !defined(_EVENT_H)
#define _EVENT_H

#include "IntrusiveBase.h"
#include "EventManager.h"

#include <list>
#include <string>
#include <boost/any.hpp>

class CycDupEventFieldErr: public CycException {
    public: CycDupEventFieldErr(std::string msg) : CycException(msg) {};
};

class Event: IntrusiveBase<Event> {
  public:
    friend class EventManager;

    // Add an arbitrary labeled value to the event.
    //
    // @param field a label or key for a value. Loosely analogous to a column
    // label.
    //
    // @warning for the val argument - what val types are supported depends on
    // what the selected backend(s) handles.
    event_ptr addVal(std::string field, boost::any val);

    // Add a timestamp value to this event of the current simulation time.
    // @return a pointer to this event (for method chaining).
    event_ptr timestamp();

    // Record this event to output.
    void record();

    std::string group();
    ValMap vals();
    std::string name();

    virtual ~Event();

  private:
    Event(EventManager* m, Model* creator, std::string group);
    bool schemaWithin(event_ptr primary);

    EventManager* manager_;
    std::string group_;
    int creator_id_;
    std::string creator_impl_;
    ValMap vals_;
};

#endif
