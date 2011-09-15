// Message.h
// A Message class for inter-entity requests, instructions, etc.

#if !defined(_MESSAGE)
#define _MESSAGE

#include "Commodity.h"
#include "Material.h"

class Communicator;

/**
 * An enumerative type to specify which direction (up or down the class 
 * hierarchy) this message is moving.
 */
enum MessageDir {UP_MSG, DOWN_MSG, NONE_MSG};

/**
 * A transaction structure to include in any message.
 */

struct Transaction
{
  /**
   * The Commodity that is being requested or offered in this Message.
   */
  Commodity* commod;

  /**
   * The amount of the specified commodity being requested or offered. 
   * Units vary. 
   * 
   * Note: positive amounts mean you want something, negative amounts 
   * mean you want to get rid of something.
   */
  double amount;

  /**
   * The minimum amount of the specified commodity being requested or offered. 
   * Units vary. 
   * 
   * Note: positive amounts mean you want something, negative amounts 
   * mean you want to get rid of something.
   */
  double min;

  /**
   * The price per unit of the commodity being requested or offered.
   */
  double price;

  /**
   * A specific composition, if this transaction requires one.
   */
  CompMap comp;

  /**
   * The ID of the Communicator who is the supplier in this transaction.
   */
  int supplierID;

  /**
   * The ID of the Communicator who is the requester in this transaction.
   */
  int requesterID;

};


/**
 * A Message class for inter-entity communication.
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
     * The Market this message will pass to or from.
     */
    Communicator* mkt_;

    /**
     * The region this messgae will pas through.
     */
    Communicator* reg_;

    /**
     * The region this messgae will pas through.
     */
    Communicator* inst_;

    /**
     * The region this messgae will pas through.
     */
    Communicator* fac_;

  public:
    /**
     * Creates an empty message from some communicator in some direction.
     *
     * @param dir the direction this Message is traveling
     * @param toSend the sender of this Message
     */
    Message(MessageDir dir, Communicator* toSend);

    /**
     * Creates a message from a direction, a transaction, 
     * a recipient, and a sender.
     */
    Message(MessageDir dir, Transaction trans, 
        Communicator* toSend, Communicator* toReceive);

    /**
     * Creates a message regarding a transaction. This is used
     * in conjunction with the BookKeeper and does not need a 
     * direction.
     * Ex: Recording the initial stocks of fuel in a StorageFacility
     */
    Message(Commodity* thisCommod, CompMap thisComp, double thisAmount, 
	    double thisPrice, double minAmt,
	    Communicator* toSend, Communicator* toReceive);

    /**
     * Creates a new Message with the given specs.
     *
     * @param dir the direction this Message is traveling
     * @param commod the Commodity being offered or requested
     * @param amount the amount of the given Commodity being offered/requested
     * Note: positive amounts mean you want something, negative amounts 
     * mean you want to get rid of something.
     * @param minAmt the minimum amount acceptible for this transaction
     * @param price the price of the Commodity
     * @param toSend the sender of this Message
     * @param toReceive the recipient of this Message, or null if the 
     * eventual recipient/handler is unknown to the sender
     * @param comp the specific composition of the material to be traded 
     */
    Message(MessageDir dir, Commodity* commod, 
        double amount, double minAmt, double price, Communicator* toSend, 
        Communicator* toReceive, CompMap comp);

    /**
     * Creates a new Message with the given specs.
     *
     * @param dir the direction this Message is traveling
     * @param commod the Commodity being offered or requested
     * @param amount the amount of the given Commodity being offered/requested
     * Note: positive amounts mean you want something, negative amounts 
     * mean you want to get rid of something.
     * @param minAmt the minimum amount acceptible for this transaction
     * @param price the price of the Commodity
     * @param toSend the sender of this Message
     * @param toReceive the recipient of this Message, or null if the 
     * eventual recipient/handler is unknown to the sender
     */
    Message(MessageDir dir, Commodity* commod, 
        double amount, double minAmt, double price, Communicator* toSend, 
        Communicator* toReceive);

    /**
     * A copy "constructor" for this class.
     *
     * @return the copy of this Message
     */
    Message* clone() const;

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
    Communicator* getSender() const;

    /**
     * Returns the recipient of this Message.
     *
     * @return the recipient
     */
    Communicator* getRecipient() const;

    /**
     * Returns the ID of the supplier in this Message.
     *
     * @return the ID of the supplier
     */
    int getSupplierID() const;

    /**
     * Returns the requester in this Message.
     *
     * @return the ID of the requester
     */
    int getRequesterID() const;

    /**
     * Returns the direction this Message is traveling.
     */
    MessageDir getDir() const;

    /**
     * Returns the transaction associated with this message.
     *
     * @return the Transaction
     */
    Transaction getTrans() const;

    /**
     * Returns the Commodity requested or offered in this Message.
     *
     * @return the Commodity
     */
    Commodity* getCommod() const;

    /**
     * Sets the Commodity requested or offered in this Message.
     *
     * @param newCommod the Commodity
     */
    void setCommod(Commodity* newCommod);

    /**
     * Returns the amount of some Commodity being requested or offered in 
     * this message.
     *
     * @return the amount (units vary)
     */
    double getAmount() const;

    /**
     * Returns the facility this message is passing to or from
     *
     * @return fac
     */
    Communicator* getFac() const;

    /**
     * Returns the institution this message is passing through
     *
     * @return inst
     */
    Communicator* getInst() const;

    /**
     * Returns the region this message is passing through
     *
     * @return reg
     */
    Communicator* getReg() const;

    /**
     * Returns the facility this message is passing to or from
     *
     * @return mkt
     */
    Communicator* getMkt() const;
    
    /**
     * Sets the direction of the message
     *
     * @param newDir is the new direction
     */
    void setDir(MessageDir newDir);

    /**
     * Sets the amount of some Commodity being requested or offered in this 
     * Message. 
     *
     * @param newAmount the updated amount
     */
    void setAmount(double newAmount);

    /**
     * Sets the assigned supplier of the material for the 
     * transaction in this message. 
     *
     * @param newID the ID of the new supplier
     */
    void setSupplierID(int newID);

    /**
     * Sets the assigned requester to receive the material
     * for the transaction in this message.
     *
     * @param newID the ID of the new requester
     */
    void setRequesterID(int newID);

    /**
     * Returns the price being requested or offered in this message.
     *
     * @return the price (in dollars)
     */
    double getPrice() const;

    /**
     * Returns the CompMap being requested or offered in this message.
     *
     * @return the CompMap (map <iso, atoms>)
     */
    CompMap getComp() const;

    /**
     * Sets the assigned composition to a new composition
     *
     * @param newComp is the new composition in the transaction
     */
    void setComp(CompMap newComp);

    /**
     * Reverses the direction this Message is being sent (so, for 
     * instance, the Manager can forward a message back down the hierarchy 
     * to an appropriate handler.
     */
    void reverseDirection();

    /**
     * @brief Set the message path using a direction, sender and recipient.
     * @todo make this a private method (rcarlsen)
     *
     */
    void setPath();

    /**
     * @brief unenumerates the message direction.
     * 
     * @return the string associated with myDir
     */
    string unEnumerateDir();

    /**
     * Executes the transaction involved in the message.
     */
    void execute();

};
#endif
