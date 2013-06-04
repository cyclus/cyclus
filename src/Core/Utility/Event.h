// Event.h
#if !defined(_EVENT_H)
#define _EVENT_H

#include "IntrusiveBase.h"
#include "EventManager.h"

#include <list>
#include <string>
#include <boost/any.hpp>

typedef boost::intrusive_ptr<Event> event_ptr;

/*!
Used to specify and send a collection of key-value pairs to the
EventManager for recording.
*/
class Event: IntrusiveBase<Event> {
    friend class EventManager;

  public:
    typedef std::pair<std::string, boost::any> Entry;
    typedef std::vector<Entry> List;

    virtual ~Event();

    /*!
    Add an arbitrary field-value pair to the event.

    @param field a label or key for a value. Loosely analogous to a column
    label.

    @param val a value of any type (int, bool, string, vector) supported by
    the backends being used.

    @warning for the val argument - what variable types are supported
    depends on what the backend(s) in use are designed to handle.

    @throw CycDupEventFieldErr the passed field has been used already in
    this event
    */
    event_ptr addVal(std::string field, boost::any val);

    /*!
    Record this event to its EventManager. Recorded events of the same
    title (e.g. same table) must not contain any fields that were not
    present in the first event recorded of that title.
    */
    void record();

    /// Returns the event's title as specified during the event's creation.
    std::string title();

    /// Returns a map of all field-value pairs that have been added to this event.
    const List& vals();

  private:
    /// events should only be created via an EventManager
    Event(EventManager* m, std::string title);
    int count_;

    EventManager* manager_;
    std::string title_;
    List vals_;
};

#endif
