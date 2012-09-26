
// EventManager.h
#if !defined(_EVENTMANAGER_H)
#define _EVENTMANAGER_H

#include "Event.h"
#include "CycException.h"
#include <list>
#include <string>

typedef std::list<event_ptr> event_list;
class CycGroupDataMismatch: public CycException {
    public: CycGroupDataMismatch(std::string msg) : CycException(msg) {};
};

class EventBackend {
  public notify(event_list events) = 0;
}

class EventManager {
  private:
    bool isValidSchema(event_ptr ev);

    std::list<event_ptr> events_;
    std::map<Model*, std::map<std::string, event_ptr> > schemas_;
    std::list<EventBackend*> backs_;
    void notifyBacks();

  public:
    event_ptr newEvent(Model* creator, std::string group);
    void registerBackend(EventBackend b);
    void addEvent(event_ptr ev);

};


