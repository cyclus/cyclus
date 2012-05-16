// Message.h
// A Message class for inter-entity requests, instructions, etc.

#if !defined(_MESSAGE)
#define _MESSAGE

#include <vector>
#include <string>

#include "Resource.h"
#include "IntrusiveBase.h"
#include "Table.h"
#include "CycException.h"

class Communicator;
class Model;
class Message;
class MarketModel;
class Transaction;

typedef boost::intrusive_ptr<Message> msg_ptr;

/**
   An enumerative type to specify which direction 
   (up or down the class hierarchy) this message is moving. 
 */
enum MessageDir {UP_MSG, DOWN_MSG, NONE_MSG};

class CycCircularMsgException: public CycException {
  public: CycCircularMsgException() :
      CycException("Message receiver and sender are the same.") { };
};

class CycNoMsgReceiverException: public CycException {
  public: CycNoMsgReceiverException() : 
      CycException("Can't send the message - must call setNextDest first") { };
};

class CycNullMsgParamException: public CycException {
  public: CycNullMsgParamException(std::string msg) : CycException(msg) {};
};

/**
   A transaction structure to include in any message. 
 */
struct Transaction {
  /**
     The commodity that is being requested or offered in this Message. 
   */
  std::string commod;

  /**
     True if this is an offer, false if it's a request 
   */
  bool is_offer;

  /**
     The minimum fraction of the specified commodity that the 
     requester is willing to accept or the offerer is willing to send. 
   */
  double minfrac;

  /**
     The price per unit of the commodity being requested or offered. 
   */
  double price;

  /**
     A specific resource with which this transaction is concerned.
   */
  rsrc_ptr resource;

  /**
     The supplier in this transaction. 
   */
  Model* supplier;

  /**
     The requester in this transaction. 
   */
  Model* requester;
};


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
   intended receiver. The message class is designed to facilitate a two 
   leg path. The first leg, the message is in an "outgoing" state. The 
   originator will specify the next stop (next communicator) to receive 
   the message and invoke the sendOn() method of the message. The next 
   stop communicator receives the message, does necessary processing, 
   sets the message's "next stop", and invokes the message's sendOn() 
   method. This process is repeated until the message direction is 
   flipped to the incoming (return leg) state. When in the incomming 
   state, a communicator invokes the sendOn() method and the message is 
   sent to the communicator from which this communicator received the 
   message. An example of the message passing is outlined below:  
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
   message are ignored. Incoming messages simply follow their upward 
   path in reverse order. This paradigm allows for an arbitrary 
   communicator to communicator path (for the outgoing leg). The message 
   is also guaranteed to retrace its path on an incoming leg provided 
   each communicator invokes the message's sendOn() method. 
    
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
  void constructBase(Communicator* sender);

 public:
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
  Message(Communicator* sender, Communicator* receiver, Transaction trans);

  virtual ~Message();

  /**
     The copy returned will contain a clone of this message's transaction (this
     is a deep copy).

     @return a newly allocated copy of this message object
   */
  msg_ptr clone();

  /**
     Send this message to the next communicator in it's path 
      
     Messages heading up (UP_MSG) are forwareded to the communicator 
     designated by the setNextDest(Communicator*) function. Messages 
     heading down (DOWN_MSG) are sent successively to each communicator 
     in reverse order of their 'upward' sequence. 
      
     @exception CycNoMsgReceiverException no receiver is designated (must call
     setNextDest first) 
      
     @exception CycCircularMsgException attempted to send a message to the 
     message sender (circular messaging) 
   */
  virtual void sendOn();

 private:

  void validateForSend();

  void makeRealParticipant(Communicator* who);
  
 public:

  /**
     Calls to this method are ignored (do nothing) when the message 
     direction is DOWN_MSG. 
      
     @param next_stop the communicator that will receive this message when
     sendOn is next invoked
   */
  void setNextDest(Communicator* next_stop);
  
  /**
     Initiate the market-matched transaction - resource(s) are taken from the
     supplier and sent to the requester.
      
     This should be the sole way of transferring resources between simulation
     agents/models. Book keeping of transactions (and corresponding resource
     states) are taken care of automatically.
   */
  void approveTransfer();
  
  /**
     Renders the sendOn method disfunctional, preventing it from being sent
     anywhere.
      
     Used to prevent messages from returning through/to deallocated Communicators
   */
  void kill();

  /**
     Indicates whether this message is active and sendable.

     @return true if this message has been killed, false otherwise - see
     Message::kill() 
   */
  bool isDead();
  
  /**
     setNextDest must be called before sendOn is invoked only if dir is UP_MSG.

     @return the direction this Message is traveling (UP_MSG or DOWN_MSG). 
   */
  MessageDir dir() const;
  
  /**
     Sets the direction of the message 
      
     @param new_dir is the new direction 
   */
  void setDir(MessageDir new_dir);
  
  /**
     @return the corresponding offer/request message assuming this message has
     been matched in a market. Returns the 'this' pointer otherwise. 
   */
  msg_ptr partner();

  /**
     Used to match this message with a corresponding offer/request message
     after matching takes place in a market. 

     Allows requesters to know which request message that they sent corresponds
     to the resources they receive.

     @param partner the matched offer/request counterpart to this message.

     @TODO figure out how to make this work with markets
   */
  void setPartner(msg_ptr partner);

  /**
     Set via the Message constructor and cannot be changed.

     @return the sender (original creator) of this Message. 
   */
  Communicator* sender() const;
  
  /**
     Set via the Message constructor and cannot be changed.

     @return the receiver of this Message, a destination set by the sender. 

     @exception CycNullMsgParamException receiver is uninitialized (NULL)
   */
  Communicator* receiver() const;

///////////////////////////////////////////////////////////////////////////////
////////////// transaction getters/setters ////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

  /**
     Get the market corresponding to the transaction commodity 
      
     @return market corresponding to this msg's transaction's commodity 
      
   */
  MarketModel* market();
  
  /**
     @return a pointer to the supplier in this Message. 

     @exception CycNullMsgParamException supplier is uninitialized (NULL)
   */
  Model* supplier() const;

  /**
     Sets the assigned supplier of the material for the 
     transaction in this message. 
      
     @param supplier pointer to the new supplier 
   */
  void setSupplier(Model* supplier);

  /**
     @return a pointer to the requester in this Message. 

     @exception CycNullMsgParamException requester is uninitialized (NULL)
   */
  Model* requester() const;

  /**
     Sets the assigned requester to receive the material 
     for the transaction in this message. 
      
     @param requester pointer to the new requester 
   */
  void setRequester(Model* requester);

  /**
    Although passed by value, the trans.resource is a pointer, and modifying it
    will change this message's transaction's resource also.

     @returns the transaction associated with this message. 
   */
  Transaction trans() const;

  /**
     Returns the commodity requested or offered in this Message. 
   */
  std::string commod() const;

  /**
     Sets the commodity requested or offered in this Message. 
      
     @param new_commod the commodity associated with this Message
   */
  void setCommod(std::string new_commod);

  /**
     True if the transaction is an offer, false if it is a request.
   */
  bool isOffer() const;

  /**
     Set the transaction type (true=offer, false=request)
   */
  void setIsOffer(bool offer);

  /**
     Returns the price (in dollars) being requested or offered in this message. 
   */
  double price() const;

  /**
     Set the price (in dollars) being requested or offered in this message. 
   */
  void setPrice(double new_price);

  /**
     Returns a pointer to the Resource being requested or offered in this message. 
   */
  rsrc_ptr resource() const;

  /**
     Sets the message transaction's resource to a copy of the passed resource.
   */
  void setResource(rsrc_ptr new_resource);

 private:
  /**
     The direction this message is traveling 
     (up or down the class hierarchy). 
   */
  MessageDir dir_;
  
  /**
     The Transaction with which this message is concerned. 
   */
  Transaction trans_;
  
  /**
     The Communicator who sent this Message. 
   */
  Communicator* sender_;
  
  /**
     The Communicator who will receive this Message. 
   */
  Communicator* receiver_;

  /**
     Pointers to each model this message passes through. 
   */
  std::vector<Communicator*> path_stack_;
  
  /**
     the most recent communicator to receive this message. 
      
     Used to prevent circular messaging. 
   */
  Communicator* curr_owner_;

  /**
     offer/request partner for this message (only for matched pairs)
   */
  msg_ptr partner_;
  
  /**
     stores the next available transaction ID 
   */
  static int next_trans_id_;

  /**
     a boolean to determine if the message has completed its route 
   */
  bool dead_;

///////////////////////////////////////////////////////////////////////////////
////////////// Output db recording code ///////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

 public:
  /**
     the transaction output database Table 
   */
  static table_ptr trans_table;

  /**
     the transacted resource output database Table 
   */
  static table_ptr trans_resource_table;

 private:
  /**
     Define the transaction database table 
   */
  static void define_trans_table();

  /**
     Define the transacted resource database table 
   */
  static void define_trans_resource_table();

  /**
     add a transaction to the transaction table 
     @param id the message id 
   */
  void addTransToTable(int id);

  /**
     add a transaction to the transaction table 
     @param id the message id 
     @param position the position in the manifest 
     @param resource the resource being transacted 
   */
  void addResourceToTable(int id, int position, rsrc_ptr resource);

  /**
     the transaction primary key 
   */
  primary_key_ref pkref_trans_;

  /**
     the resource primary key 
   */
  primary_key_ref pkref_rsrc_;
};

#endif
