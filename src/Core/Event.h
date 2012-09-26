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

    event_ptr addVal(std::string field, boost::any val);
    event_ptr timestamp();
    void record();

    std::string group();
    Model* creator();
    ValMap vals();

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
