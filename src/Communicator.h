// Communicator.h
#if !defined(_COMMUNICATOR_H)
#define _COMMUNICATOR_H

#include "Message.h"

/**
 *  @brief An abstract class for deriving simulation entities that can communicate via the Message class.
 *
 *  This base class was intentionally kept minimal.
 *  This class header explicitly includes Message.h so that all
 *  Communicator derived classes automatically get this definition for their
 *  own use.
*/
class Communicator {
  
public:
  virtual ~Communicator() { };

  friend class Message;

private:

  /**
   *  @brief Models communicate desires for material, etc. by sending
   *         and receiveing messages.
   *
   *  @param msg pointer to message to be received
   */
  virtual void receiveMessage(Message* msg) = 0;

protected:

  /// Copy the base class data members from one object to another
  virtual void copy(Communicator* src) { };

};
#endif

