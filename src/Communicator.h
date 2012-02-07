// Communicator.h
#if !defined(_COMMUNICATOR_H)
#define _COMMUNICATOR_H

#include "Message.h"

/**
   @brief An abstract class for deriving simulation entities 
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
   defined in the CommunicatorClass, but these behaviors may be overriden 
   by the specific model implementation. These functions include:
   - receiveMessage
   
   @section seeAlso See Also
   The MessageClass describes the structure and interface of messages 
   passed between communicators. The StubCommModel provides an example 
   of a Communicator model implementation.
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
   *  @warning This method should never be called directly by any Model object.
   *           Message sending should be handled via methods on the Message class.
   */
  virtual void receiveMessage(msg_ptr msg) = 0;

protected:

  /// Copy the base class data members from one object to another
  virtual void copy(Communicator* src) { };

};
#endif

