
// EventManager.h
#if !defined(_EVENTMANAGER_H)
#define _EVENTMANAGER_H

#include "Event.h"
#include "CycException.h"
#include <list>
#include <string>

#define EM EventManager::Instance()

typedef std::list<event_ptr> EventList;
class CycGroupDataMismatch: public CycException {
    public: CycGroupDataMismatch(std::string msg) : CycException(msg) {};
};

class EventBackend {
  public void notify(EventList events) = 0;
  public std::string name() = 0;
  public void close() = 0;
}

class EventManager {
  private:
    bool isValidSchema(event_ptr ev);
    void notifyBacks();

    std::list<event_ptr> events_;
    std::map<Model*, std::map<std::string, event_ptr> > schemas_;
    std::list<EventBackend*> backs_;

    /// A pointer to singleton EventManager.
    static BookKeeper* instance_;

  public:
    event_ptr newEvent(Model* creator, std::string group);
    void registerBackend(EventBackend b);
    void addEvent(event_ptr ev);
    void close();

    /// Returns a pointer to the global singleton EventManager
    static EventManager* Instance();

};


