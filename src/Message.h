// Message.h
// A Message class for inter-entity requests, instructions, etc.

#if !defined(_MESSAGE)
# define _MESSAGE

#include "Commodity.h"

class Communicator;

/**
 * An enumerative type to specify which direction (up or down the class 
 * hierarchy) this message is moving.
 */
enum MessageDir {up, down};

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
   * The price of the commodity being requested or offered.
   */
  double price;

  /**
   * The Communicator who is the supplier in this transaction.
   */
  Communicator* supplier;

  /**
   * The Communicator who is the requester in this transaction.
   */
  Communicator* requester;
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
    MessageDir dir;

    /**
     * The Transaction this message is concerned with
     */
    Transaction trans;

    /**
     * The Communicator who sent this Message.
     */
    Communicator* sender;

    /**
     * The Communicator who will receive this Message.
     */
    Communicator* recipient;

    /**
     * The Market this message will pass to or from.
     */
    Communicator* mkt;

    /**
     * The region this messgae will pas through.
     */
    Communicator* reg;

    /**
     * The region this messgae will pas through.
     */
    Communicator* inst;

    /**
     * The region this messgae will pas through.
     */
    Communicator* fac;

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
     * Returns the supplier in this Message.
     *
     * @return the supplier
     */
    Communicator* getSupplier() const;

    /**
     * Returns the requester in this Message.
     *
     * @return the requester
     */
    Communicator* getRequester() const;

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
     * @param newSupplier the assigned supplier
     */
    void setSupplier(Communicator* newSupplier);

    /**
     * Sets the assigned requester to receive the material
     * for the transaction in this message.
     *
     * @param newRequester the updated requester
     */
    void setRequester(Communicator* newRequester);

    /**
     * Returns the price being requested or offered in this message.
     *
     * @return the price (in dollars)
     */
    double getPrice() const;

    /**
     * Reverses the direction this Message is being sent (so, for 
     * instance, the Manager can forward a message back down the hierarchy 
     * to an appropriate handler.
     */
    void reverseDirection();

    /**
     * Sets the path of the message using the direction, sender and recipient.
     *
     * @param dir the direction of the message
     * @param sender the sender of the message
     * @param recipient the recipient of the message
     */
    void setPath(MessageDir dir, Communicator* sender, Communicator* recipient);

    /**
     * unEnumerates the message direction.
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
