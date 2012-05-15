// Message.h
// A Message class for inter-entity requests, instructions, etc.

#if !defined(_MESSAGE)
#define _MESSAGE

#include <vector>
#include <string>
#include "boost/intrusive_ptr.hpp"

#include "Model.h"
#include "Resource.h"
#include "IntrusiveBase.h"
#include "Table.h"
#include "CycException.h"

class Communicator;
class Model;
class Message;
class MarketModel;

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
     A specific resource this transaction is concerned with 
   */
  rsrc_ptr resource;

  /**
     supplier in this transaction. 
   */
  Model* supplier;

  /**
     requester in this transaction. 
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
   -# msg->setNextDest(nextStop) 
   -# msg->sendOn() 
   -# message object invokes receiveMessage(this) for nextStop 
   -# Inside stop A 
   -# msg->setNextDest(nextStop) 
   -# msg->sendOn() 
   -# message object invokes receiveMessage(this) for nextStop 
   -# Inside stop B 
   -# msg->setNextDest(nextStop) 
   -# msg->sendOn() 
   -# message object invokes receiveMessage(this) for nextStop 
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
     Creates a new message by copying the current one and 
     returns a reference to it. 
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

  /**
     Renders the sendOn method disfunctional. 
      
     Used to prevend messages from returning through/to deallocated 
   */
  void kill();

  /**
     returns true if this message has been killed - see Message::kill() 
   */
  bool isDead();
  
  /**
     designate the next object to receive this message 
      
     Calls to this method are ignored (do nothing) when the message 
     direction is down. 
      
     @param nextStop the next communicator to receive this message 
      
   */
  void setNextDest(Communicator* nextStop);
  
  /**
     Reverses the direction this Message is being sent (up to down or down
     to up).
   */
  void reverse();
  
  /**
     Returns the direction this Message is traveling (UP_MSG or DOWN_MSG). 
   */
  MessageDir dir() const;
  
  /**
     Sets the direction of the message 
      
     @param newDir is the new direction 
   */
  void setDir(MessageDir newDir);
  
  /**
     Get the market corresponding to the transaction commodity 
      
     @return market corresponding to this msg's transaction's commodity 
      
   */
  MarketModel* market();
  
  /**
     Returns the sender of this Message. 
   */
  Communicator* sender() const {return sender_;};
  
  /**
     Returns the receiver of this Message. 

     @exception CycNullMsgParamException receiver is uninitialized (NULL)
   */
  Communicator* receiver() const;

  /**
     Returns a pointer to the supplier in this Message. 

     @exception CycNullMsgParamException supplier is uninitialized (NULL)
   */
  Model* supplier() const;

  /**
     Sets the assigned supplier of the material for the 
     transaction in this message. 
      
     @param supplier pointer to the new supplier 
   */
  void setSupplier(Model* supplier) {trans_.supplier = supplier;};

  /**
     Returns a pointer to the requester in this Message. 

     @exception CycNullMsgParamException requester is uninitialized (NULL)
   */
  Model* requester() const;

  /**
     Sets the assigned requester to receive the material 
     for the transaction in this message. 
      
     @param requester pointer to the new requester 
   */
  void setRequester(Model* requester) {trans_.requester = requester;};

  /**
     Returns the transaction associated with this message. 
   */
  Transaction trans() const {return trans_;};

  /**
     Returns the commodity requested or offered in this Message. 
   */
  std::string commod() const {return trans_.commod;};

  /**
     Sets the commodity requested or offered in this Message. 
      
     @param new_commod the commodity associated with this 
   */
  void setCommod(std::string newCommod) {trans_.commod = newCommod;};

  /**
     True if the transaction is an offer, false if it is a request.
   */
  bool isOffer() const {return trans_.is_offer;};

  /**
     Set the transaction type (true=offer, false=request)
   */
  void setIsOffer(bool offer) {trans_.is_offer = offer;};

  /**
     Returns the price (in dollars) being requested or offered in this message. 
   */
  double price() const {return trans_.price;};

  /**
     Set the price (in dollars) being requested or offered in this message. 
   */
  void setPrice(double newPrice) {trans_.price = newPrice;};

  /**
     Returns a pointer to the Resource being requested or offered in this message. 
   */
  rsrc_ptr resource() const {return trans_.resource;};

  /**
     Sets the message transaction's resource to a copy of the passed resource.
   */
  void setResource(rsrc_ptr newResource) {if (newResource.get()) {trans_.resource = newResource->clone();}};

  /**
     Used to match this message with a corresponding offer/request message
     after matching takes place in a market. 

     Allows requesters to know which request message that they sent corresponds
     to the resources they receive.

     @TODO figure out how to make this work with markets
   */
  void setPartner(msg_ptr partner) {partner_ = partner;};

  /**
     returns the corresponding offer/request message assuming this message has
     been matched in a market. Returns the 'this' pointer otherwise. 
   */
  msg_ptr partner() {return partner_;};

  /**
     Initiate the market-matched transaction - resource(s) are taken from the
     supplier and sent to the requester.
      
     This should be the sole way of transferring resources between simulation
     agents/models. Book keeping of transactions (and corresponding resource
     states) are taken care of automatically.
   */
  void approveTransfer();
  
 private:
  /**
     The direction this message is traveling 
     (up or down the class hierarchy). 
   */
  MessageDir dir_;
  
  /**
     The Transaction this message is concerned with 
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
  std::vector<Communicator*> pathStack_;
  
  /**
     the most recent communicator to receive this message. 
      
     Used to prevent circular messaging. 
   */
  Communicator* currentOwner_;

  /**
     offer/request partner for this message (meaning only for matched 
   */
  msg_ptr partner_;
  
  /**
     Checks required conditions prior to sending a message. 
   */
  void validateForSend();

  /**
     mark a Model* as a participating sim agent (not a template) 
   */
  void makeRealParticipant(Communicator* who);
  
  /**
     stores the next available transaction ID 
   */
  static int nextTransID_;

  /**
     a boolean to determine if the message has completed its route 
   */
  bool dead_;


// -------- output database related members  -------- 
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
// -------- output database related members  --------   
};

#endif
