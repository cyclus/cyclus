// communicator.h
#if !defined(_COMMUNICATOR_H)
#define _COMMUNICATOR_H

#include <set>

#include "message.h"
#include "logger.h"

namespace cyclus {

/**
   An abstract class for deriving simulation entities
   that can communicate via the Message class.

   @section introduction Introduction
   The CommunicatorClass must be inherited by all models
   that pass messages during the simulation.

   @section commModels Communicator Models
   Classes which should inherent CommunicatorClass functionality
   include (but may not be limited to) RegionModel, InstModel,
   FacilityModel and MarketModel classes. These model classes pass
   messages during the simulation. These include offers and requests
   for materials as well as completed transactions.

   Each Communicator instance is assigned an ID so that messages
   can be passed explicitly from one communicator to another.

   @section interface Interface
   The functionality provided by the CommunicatorClass includes a set
   of sending and receiving functions. These have default behaviors
   defined in the CommunicatorClass, but these behaviors may be
   overriden by the specific model implementation. These functions
   include: - receiveMessage

   @section seeAlso See Also
   The MessageClass describes the structure and interface of messages
   passed between communicators. The StubCommModel provides an example
   of a Communicator model implementation.
 */
class Communicator {

 public:
  virtual ~Communicator() {
    MLOG(LEV_DEBUG4) << "communicator " << this << " destructed";
    std::set<Message::Ptr>::iterator it;
    for (it = tracked_.begin(); it != tracked_.end(); it++) {
      (*it)->Kill();
      LOG(LEV_DEBUG3, "delete") << "killing tracked message";
    }
    MLOG(LEV_DEBUG4) << "communicator " << this << " destructed";
  };

  friend class Message;

 private:

  /**
     Models communicate desires for material, etc. by sending
     and receiveing messages.

     @param msg pointer to message to be received
     @warning This method should never be called directly by any Model
     object. Message sending should be handled via methods on the
   */
  virtual void ReceiveMessage(Message::Ptr msg) = 0;

  std::set<Message::Ptr> tracked_;

  /**
     Add msg to a list of msgs to be killed when this communicator is
     deallocated. This functionality is used to prevent messages from
     attempting to return themselves either to or through communicator
     objects that have been deallocated.
     @param msg the Message to be tracked.
   */
  void TrackMessage(Message::Ptr msg) {
    tracked_.insert(msg);
    MLOG(LEV_DEBUG5) << "communicator " << this << " tracks Message " << msg;
  }

  /**
     Remove msg from a list of msgs to be killed when this communicator
     is deallocated. This functionality is used to prevent messages from
     attempting to return themselves either to or through communicator
     objects that have been deallocated.
     @param msg the Message to untrack
   */
  void UntrackMessage(Message::Ptr msg) {
    tracked_.erase(msg);
    MLOG(LEV_DEBUG5) << "communicator " << this << " untracked Message " << msg;
  }

 protected:
  /**
     Copy the base class data members from one object to another

     @param src the Communicator to copy
   */
  virtual void Copy(Communicator* src) { };

};
} // namespace cyclus
#endif

