// Message.h
// A Message class for inter-entity requests, instructions, etc.

#if !defined(_MESSAGE)
#define _MESSAGE

#include "Model.h"
#include "Resource.h"
#include <vector>
#include <string>

class Communicator;
class Model;

/**
 * An enumerative type to specify which direction (up or down the class 
 * hierarchy) this message is moving.
 */
enum MessageDir {UP_MSG, DOWN_MSG, NONE_MSG};

/**
 * A transaction structure to include in any message.
 */

struct Transaction {

  /**
     The transaction's unique ID
   */
  int ID;
  
  /**
   * The commodity that is being requested or offered in this Message.
   */
  std::string commod;

  /**
   * True if this is an offer, false if it's a request
   */
  bool is_offer;

  /**
   * The minimum fraction of the specified commodity that the 
   * requester is willing to accept or the offerer is willing to send. 
   */
  double minfrac;

  /**
   * The price per unit of the commodity being requested or offered.
   */
  double price;

  /**
   * A specific resource this transaction is concerned with 
   */
  Resource* resource;

  /**
   * @brief supplier in this transaction.
   */
  Model* supplier;

  /**
   * @brief requester in this transaction.
   */
  Model* requester;

};


/**
   @brief A Message class for inter-entity communication.

   @section intro Introduction
   The MessageClass describes the structure of messages that 
   CommunicatorClass models pass during the simulation.

   @section msgStructure Message Data Structure
   Messages have a path and contain a transaction data structure. The path 
   directs the movement of the message through the appropriate channels and 
   the transaction includes information about the material order to be 
   offered, requested, or filled.

   @section path Path
   A message contains a reference (pointer) to its originator and the 
   intended receiver. The message class is designed to facilitate a two leg 
   path. The first leg, the message is in an "outgoing" state. The originator 
   will specify the next stop (next communicator) to receive the message and 
   invoke the sendOn() method of the message. The next stop communicator 
   receives the message, does necessary processing, sets the message's next 
   "next stop", and invokes the message's sendOn() method. This process is 
   repeated until the message direction is flipped to the incoming (return 
   leg) state. When in the incomming state, a communicator invokes the 
   sendOn() method and the message is sent to the communicator from which 
   this communicator received the message. An example of the message passing 
   is outlined below:

   - Up/outgoing message:
     -# Inside originator
       -# msg->setNextDest(next_stop)
     -# msg->sendOn()
     -# message object invokes receiveMessage(this) for next_stop
     -# Inside stop A
       -# msg->setNextDest(next_stop)
       -# msg->sendOn()
     -# message object invokes receiveMessage(this) for next_stop
     -# Inside stop B
       -# msg->setNextDest(next_stop)
       -# msg->sendOn()
     -# message object invokes receiveMessage(this) for next_stop
     -# Inside stop C
       -# flip message direction
   - Down/incoming message:
     -# Inside stop C
       -# msg->sendOn()
     -# message object invokes receiveMessage(this) for stop B
     -# Inside stop B
       -# msg->sendOn()
     -# message object invokes receiveMessage(this) for stop A
     -# message arrives back at its originating communicator.

   Note that all attempts to specify a "next stop" for a down/incoming 
   message are ignored. Incoming messages simply follow their upward path in 
   reverse order. This paradigm allows for an arbitrary communicator to 
   communicator path (for the outgoing leg). The message is also guaranteed 
   to retrace its path on an incoming leg provided each communicator invokes 
   the message's sendOn() method.

   In order for a message to arrive at the destination as intended by its 
   originator, each communicator may check to see if it is the recipient. If 
   not, it should continue forwarding the message in some defined way 
   (usually up a hierarchy). Attempts to invoke sendOn() without first 
   specifying a next stop will throw an exception (this helps prevent 
   circular/recursive messaging). An exception is also thrown if a 
   communicator attempts to send a message after the message has completed an 
   entire 2-leg round trip

   @section transaction Transaction
   The transaction contains information about the material order to be 
   offered, requested, or filled. Sufficient information for specifying the 
   transaction follows:
   - the amount of the given Commodity being offered/requested
   - the minimum amount acceptible for this transaction
   - the price of the Commodity
   - the specific resource to be traded
   
   Communicator classes which should utilize the MessageClass interface 
   include RegionModel, InstModel, FacilityModel and MarketModel classes. 
   These model classes pass messages during the simulation.

   @section seeAlso See Also
   The CommunicatorClass describes the class of models that pass messages 
   during the simulation.. The StubCommModel provides an example of a Message 
   model implementation.
 */

class Message {
 private:
  /**
   * The direction this message is traveling (up or down the class 
   * hierarchy).
   */
  MessageDir dir_;
  
  /**
   * The Transaction this message is concerned with
   */
  Transaction trans_;
  
  /**
   * The Communicator who sent this Message.
   */
  Communicator* sender_;
  
  /**
   * The Communicator who will receive this Message.
   */
  Communicator* recipient_;
  
  /**
   * Pointers to each model this message passes through.
   */
  std::vector<Communicator*> path_stack_;
  
  /**
   * @brief the most recent communicator to receive this message.
   *
   * Used to prevent circular messaging.
   */
  Communicator* current_owner_;
  
  /**
   * @brief Checks required conditions prior to sending a message.
   *
   */
  void validateForSend();
  
 protected:
    
  /**
     @brief stores the next available message (transaction) ID
  */
  static int nextID_;

 public:
  
  /**
   * Creates an empty upward message from some communicator.
   *
   * @param sender the sender of this Message
   */
  Message(Communicator* sender);

  /**
   * Creates an upward message using the given
   * sender, and recipient.
   *
   * @param sender sender of this Message
   * @param receiver recipient of this Message
   */
  Message(Communicator* sender, Communicator* receiver);
  
  /**
   * Creates an upward message using the given sender, 
   * recipient, and transaction.
   *
   * @param sender sender of this Message
   * @param receiver recipient of this Message
   * @param trans the message's transaction specifics
   */
  Message(Communicator* sender, Communicator* receiver, Transaction trans);
  
  /**
   * Creates a new message by copying the current one and
   * returns a reference to it.
   *
   * @warning don't forget to delete the pointer when you're done.
   */
  Message* clone();

  /**
   * @brief Send this message to the next communicator in it's path
   *
   * Messages heading up (UP_MSG) are forwareded to the communicator
   * designated by the setNextDest(Communicator*) function. Messages
   * heading down (DOWN_MSG) are sent successively to each communicator
   * in reverse order of their 'upward' sequence.
   *
   * @exception CycException attempted to send message with
   *            with no designated receiver (next dest is undefined)
   *
   * @exception CycException attempted to send a message to the message
   *            sender (circular messaging)
   */
  virtual void sendOn();
  
  /**
   * @brief designate the next object to receive this message
   * 
   * Calls to this method are ignored when the message direction is
   * down.
   *
   * @param next_stop the next communicator to receive this message
   *
   */
  void setNextDest(Communicator* next_stop);
  
  /**
   * Reverses the direction this Message is being sent (so, for 
   * instance, the Manager can forward a message back down the hierarchy 
   * to an appropriate handler.
   */
  void reverseDirection();
  
  /**
   * Returns the direction this Message is traveling.
   */
  MessageDir getDir() const;
  
  /**
   * Sets the direction of the message
   *
   * @param newDir is the new direction
   */
  void setDir(MessageDir newDir);
  
  /**
   * @brief Get the market corresponding to the transaction commodity
   *
   * @return market corresponding to this msg's transaction's commodity
   *
   */
  Communicator* getMarket();
  
  /**
   * Prints the transaction data.
   *
   */
  void printTrans();
  
  /**
   * Returns the sender of this Message.
   *
   * @return the sender
   */
  Communicator* getSender() const {return sender_;};
  
  /**
   * Returns the recipient of this Message.
   *
   * @return the recipient
   */
  Communicator* getRecipient() const;

  /**
    Sets the assigned ID to the message.
   
    @param id is desired message unique id 
   */
  void setID() {trans_.ID = nextID_++;};

  /**
    Returns the Message (transaction) ID.
   
    @return message (transaction) ID
   */
  int getID() const {return trans_.ID;};

  /**
   * Returns the supplier in this Message.
   *
   * @return pointer to the supplier
   */
  Model* getSupplier() const;

  /**
   * Sets the assigned supplier of the material for the 
   * transaction in this message. 
   *
   * @param supplier pointer to the new supplier
   */
  void setSupplier(Model* supplier) {trans_.supplier = supplier;};

  /**
   * Returns the requester in this Message.
   *
   * @return pointer to the requester
   */
  Model* getRequester() const;

  /**
   * Sets the assigned requester to receive the material
   * for the transaction in this message.
   *
   * @param requester pointer to the new requester
   */
  void setRequester(Model* requester){trans_.requester = requester;};

  /**
   * Returns the transaction associated with this message.
   *
   * @return the Transaction
   */
  Transaction getTrans() const {return trans_;};

  /**
   * Returns the commodity requested or offered in this Message.
   *
   * @return commodity for this transaction
   */
  std::string commod() const {return trans_.commod;};

  /**
   * Sets the commodity requested or offered in this Message.
   *
   * @param new_commod the commodity associated with this message/transaction
   */
  void setCommod(std::string new_commod) {trans_.commod = new_commod;};

  /**
   * True if the transaction is an offer, false if it's a request
   *
   * @return true if the transaction is an offer, false if it's a request
   */
  double isOffer() const {return trans_.is_offer;};

  /**
   * True if the transaction is an offer, false if it's a request
   *
   * @return true if the transaction is an offer, false if it's a request
   */
  void setIsOffer(bool is_offer) {trans_.is_offer = is_offer;};

  /**
   * Returns the price being requested or offered in this message.
   *
   * @return the price (in dollars)
   */
  double getPrice() const {return trans_.price;};

  /**
   * Returns the price being requested or offered in this message.
   *
   * @param new_price the new price (in dollars)
   */
  void setPrice(double new_price) {trans_.price = new_price;};

  /**
   * Returns the Resource being requested or offered in this message.
   *
   * @return the Resource  (i.e. Material object) 
   */
  Resource* getResource() const {return trans_.resource;};

  /**
   * Sets the assigned resource to a new resource
   *
   * @param new_resource is the new Resource in the transaction
   */
  void setResource(Resource* new_resource) {trans_.resource = new_resource->clone();};

  /*!
  @brief Initiate the transaction - sending/receiving of resource(s) between
  the supplier/requester

  This should be the sole way of transferring resources between simulation
  agents/models. Book keeping of transactions (and corresponding resource
  states) are taken care of automatically within this method.
  */
  void approveTransfer();

};

#endif
