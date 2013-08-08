// message.h
// A Message class for inter-entity requests, instructions, etc.

#if !defined(_MESSAGE_H)
#define _MESSAGE_H

#include <vector>
#include <string>

#include "resource.h"
#include "transaction.h"
#include "intrusive_base.h"

namespace cyclus {

class Communicator;
class Transaction;

/**
   An enumerative type to specify which direction
   (up or down the class hierarchy) this message is moving.
 */
enum MessageDir {UP_MSG, DOWN_MSG, NONE_MSG};

/**
   A Message class for inter-entity communication.

   @section intro Introduction

   The MessageClass describes the structure of messages that
   CommunicatorClass models pass during the simulation.

   @section msgStructure Message Data Structure

   Messages have a path and contain a transaction data structure. The
   path directs the movement of the message through the appropriate
   channels and the transaction includes information about the material
   order to be offered, requested, or filled.

   @section path Path

   A message contains a reference (pointer) to its originator and the
   intended receiver. The message class is designed to facilitate a two leg
   path. The first leg, the message is in an "outgoing" state. The originator
   will specify the next stop (next communicator) to receive the message and
   invoke the SendOn() method of the message. The next stop communicator
   receives the message, does necessary processing, sets the message's "next
   stop", and invokes the message's SendOn() method. This process is repeated
   until the message direction is flipped to the incoming (return leg) state.
   The message will attempt to auto set the next destination to the current
   owner's parent (see sendOn API documentation).  When in the incomming
   state, a communicator invokes the SendOn() method and the message is sent
   to the communicator from which this communicator received the message. An
   example of the message passing is outlined below:

   - Up/outgoing message:
   -# Inside originator
   -# msg->SetNextDest(next_stop)
   -# msg->SendOn()
   -# message object invokes ReceiveMessage(this) for next_stop
   -# Inside stop A
   -# msg->SetNextDest(next_stop)
   -# msg->SendOn()
   -# message object invokes ReceiveMessage(this) for next_stop
   -# Inside stop B
   -# msg->SetNextDest(next_stop)
   -# msg->SendOn()
   -# message object invokes ReceiveMessage(this) for next_stop
   -# Inside stop C
   -# flip message direction
   - Down/incoming message:
   -# Inside stop C
   -# msg->SendOn()
   -# message object invokes ReceiveMessage(this) for stop B
   -# Inside stop B
   -# msg->SendOn()
   -# message object invokes ReceiveMessage(this) for stop A
   -# message arrives back at its originating communicator.

   Note that all attempts to specify a "next stop" for a down/incoming
   message are ignored. Incoming messages simply follow their upward
   path in reverse order. This paradigm allows for an arbitrary
   communicator to communicator path (for the outgoing leg). The message
   is also guaranteed to retrace its path on an incoming leg provided
   each communicator invokes the message's SendOn() method.

   In order for a message to arrive at the destination as intended by
   its originator, each communicator may check to see if it is the
   receiver. If not, it should continue forwarding the message in some
   defined way (usually up to its parent).

   @section transaction Transaction
   The transaction contains information about the material order to be
   offered, requested, or filled. Sufficient information for specifying
   the transaction follows:
   - the amount of the given Commodity being offered/requested
   - the minimum amount acceptible for this transaction
   - the price of the Commodity
   - the specific resource to be traded

   Communicator classes that utilize the Message class to communicate with
   each other during the simulation include RegionModel, InstModel,
   FacilityModel and MarketModel classes.
 */
class Message: IntrusiveBase<Message> {

 private:
  void ConstructBase(Communicator* sender);

 public:
  typedef boost::intrusive_ptr<Message> Ptr;

  /**
     Creates an empty upward message from some communicator.

     @param sender the sender of this Message
   */
  Message(Communicator* sender);

  /**
     Creates an upward message using the given
     sender, and receiver.

     @param sender sender of this Message
     @param receiver receiver of this Message
   */
  Message(Communicator* sender, Communicator* receiver);

  /**
     Creates an upward message using the given sender,
     receiver, and transaction.

     @param sender sender of this Message
     @param receiver receiver of this Message
     @param trans the message's transaction specifics
   */
  Message(Communicator* sender, Communicator* receiver, Transaction& trans);

  virtual ~Message();

  /**
     The copy returned will contain a clone of this message's transaction.
     Sender, receiver, and any previously traversed path are preserved.

     @return a newly allocated copy of this message object
   */
  Ptr clone();

  /**
     Send this message to the next communicator in it's path

     Messages heading up (UP_MSG) are forwareded to the communicator
     designated by the SetNextDest(Communicator*) function. If setNextDest
     hasn't been called, the current owner's parent model will be used as the
     next destination.  If the current owner is a root (region) node, the
     next destination will be auto set to the message's specified receiver.
     Messages heading down (DOWN_MSG) are sent successively to each
     communicator in reverse order of their 'upward' sequence.

     @exception Error no receiver is designated (must call
     setNextDest first)

     @exception Error attempted to send a message to the
     message sender (circular messaging)
   */
  virtual void SendOn();

 private:

  void AutoSetNextDest();

  /**
   Keeps history of total order vs request qtys for every commodity.
   If you need more, read the implementation - it is only 10 lines.

   @param next_model the model queued to receive this message on the next
   send
  */
  void TallyOrder(Model* next_model);

  void ValidateForSend();

 public:

  /**
     Calls to this method are ignored (do nothing) when the message
     direction is DOWN_MSG.

     @param next_stop the communicator that will receive this message when
     sendOn is next invoked
   */
  void SetNextDest(Communicator* next_stop);

  /**
     Renders the sendOn method disfunctional, preventing it from being sent
     anywhere.

     Used to prevent messages from returning through/to deallocated Communicators
   */
  void Kill();

  /**
     Indicates whether this message is active and sendable.

     @return true if this message has been killed, false otherwise - see
     Message::Kill()
   */
  bool IsDead();

  /**
     setNextDest must be called before sendOn is invoked only if dir is UP_MSG.

     @return the direction this Message is traveling (UP_MSG or DOWN_MSG).
   */
  MessageDir Dir() const;

  /**
     Sets the direction of the message

     @param new_dir is the new direction
   */
  void SetDir(MessageDir new_dir);

  /**
  Retrieve notes associated with this message.

  @return (potentially serialized) notes pertinent to the message's purpose
  */
  std::string notes();

  /**
  Add extra info that may or may not be related to a transaction.

  @param text (potentially serialized) notes pertinent to the message's purpose
  */
  void SetNotes(std::string text);

  /**
     Set via the Message constructor and cannot be changed.

     @return the sender (original creator) of this Message.
   */
  Communicator* sender() const;

  /**
     Set via the Message constructor and cannot be changed.

     @return the receiver of this Message, a destination set by the sender.
   */
  Communicator* receiver() const;

  /**
     Returns by reference the transaction associated with this message.
   */
  Transaction& trans() const;

  /**
  Allows peeking in at a commodity's supply/demand balance at specific
  simulation times.

  Note that the demand/supply balance for each timestep is maintained
  throughout the duration of the simulation.

  @warning unmetDemand is ill-defined for the current timestep - as not all
           offers/requests for the timestep in progress may have been
           generated/recorded yet.

  @param commod the commodity (i.e. market) of interest
  @param time simulation time/instant of interest

  @return the difference between the total request quantity and total offer
          quantity (negative values indicate a supply heavy state)

  */
  static double UnmetDemand(std::string commod, int time);

 private:

  /**
     The direction this message is traveling (up/down the parent/child
     hierarchy).
   */
  MessageDir dir_;

  /// The Transaction with which this message is concerned.
  Transaction* trans_;

  /// sender/creator of this Message.
  Communicator* sender_;

  /// The intended receiver of this message
  Communicator* receiver_;

  /// optional extra info that may or may not be transaction related.
  std::string notes_;

  /// Pointers to each model this message passes through.
  std::vector<Communicator*> path_stack_;

  /// the most recent communicator to receive this message.
  Communicator* curr_owner_;

  /// a boolean to determine if the message has completed its route
  bool dead_;

  static std::map<std::string, std::map<int, double> > offer_qtys_;
  static std::map<std::string, std::map<int, double> > request_qtys_;
};
} // namespace cyclus
#endif
