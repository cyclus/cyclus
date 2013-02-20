// Event.h
#if !defined(_EVENT_H)
#define _EVENT_H

#include "IntrusiveBase.h"
#include "EventManager.h"

#include <list>
#include <string>
#include <boost/any.hpp>

typedef std::map<std::string, boost::any> ValMap;

/// indicates a field has previously been added to an event.
class CycDupEventFieldErr: public CycException {
  public:
    CycDupEventFieldErr(std::string msg) : CycException(msg) {};
};

/*!
Used to specify and send a collection of key-value pairs to the
EventManager for recording.
*/
class Event: IntrusiveBase<Event> {
    friend class EventManager;

  public:

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
    name (e.g. same table) must not contain any fields that were not
    present in the first event recorded of that name.  They must also not
    contain any fields of the same name but different value type.

    @throw CycInvalidSchemaErr inconsistent event field-valtype schemas
    */
    void record();

    /// Returns the event's title as specified during the event's creation.
    std::string title();

    /// Returns a map of all field-value pairs that have been added to this event.
    ValMap vals();

    /*!
    Returns the full, unique name generated for this event using info about
    its creator along with its title.
    */
    std::string name();

  private:
    /// events should only be created via an EventManager
    Event(EventManager* m, Model* creator, std::string title);

    /// checks whether this event schema is valid within the schema in primary.
    bool schemaWithin(event_ptr primary);

    EventManager* manager_;
    std::string title_;
    int creator_id_;
    std::string creator_impl_;
    ValMap vals_;
};

#endif
