
// EventManager.h
#if !defined(_EVENTMANAGER_H)
#define _EVENTMANAGER_H

#include "CycException.h"

#include <list>
#include <string>
#include <map>
#include <boost/intrusive_ptr.hpp>
#include <boost/any.hpp>

#define EM EventManager::Instance()

class Event;
class EventManager;
class Model;

typedef boost::intrusive_ptr<Event> event_ptr;
typedef std::map<std::string, boost::any> ValMap;
typedef std::list<event_ptr> EventList;

class CycGroupDataMismatchErr: public CycException {
    public: CycGroupDataMismatchErr(std::string msg) : CycException(msg) {};
};

class EventBackend {
  public:
    virtual void notify(EventList events) = 0;
    virtual std::string name() = 0;
    virtual void close() = 0;
};

class EventManager {
  private:
    bool isValidSchema(event_ptr ev);
    void notifyBacks();

    Model* creator_;
    std::string group_;
    std::list<event_ptr> events_;
    std::map<Model*, std::map<std::string, event_ptr> > schemas_;
    std::list<EventBackend*> backs_;

    /// A pointer to singleton EventManager.
    static EventManager* instance_;

  public:
    event_ptr newEvent(Model* creator, std::string group);
    void registerBackend(EventBackend* b);
    void addEvent(event_ptr ev);
    void close();

    /// Returns a pointer to the global singleton EventManager
    static EventManager* Instance();

};

#endif
