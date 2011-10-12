// Communicator.h
#if !defined(_COMMUNICATOR_H)
#define _COMMUNICATOR_H

#include "Message.h"

/**
 *  @brief Used for determining message passing paths
 *
 *  The types of Communicators (Market, Regions, Insts, and Facilities).
 */
enum CommunicatorType {STUB_COMM, MARKET_COMM, REGION_COMM, INST_COMM, 
                        FACILITY_COMM};

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
  
  /**
   * @brief Communicator Destructor.
   */
  virtual ~Communicator();

  friend class Message;

protected:

  /**
   *  @brief Default (trivial) receiver is implemented to ignore messages
   *
   *  @param msg the message to be received
   */
  virtual void receiveMessage(Message* msg);

protected:

  /// Copy the base class data members from one object to another
  virtual void copy(Communicator* src) { };

};
#endif

