// Message.h
// A Message class for inter-entity requests, instructions, etc.

#if !defined(_MESSAGE)
#define _MESSAGE

#include "Commodity.h"
#include "Material.h"
#include <vector>

class Communicator;

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
   * @brief supplier in this transaction.
   */
  Model* supplier;

  /**
   * @brief requester in this transaction.
   */
  Model* requester;

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
     * Creates a new message by copying the current one and
     * returns a reference to it.
     *
     * @warning don't forget to delete the pointer when you're done.
     */
    Message* clone();

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
     * @return pointer to the supplier
     */
    Model* getSupplier() const;

    /**
     * Returns the requester in this Message.
     *
     * @return pointer to the requester
     */
    Model* getRequester() const;

    /**
     * Returns the transaction associated with this message.
     *
     * @return the Transaction
     */
    Transaction trans() const;

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
     * @param supplier pointer to the new supplier
     */
    void setSupplier(Model* supplier);

    /**
     * Sets the assigned requester to receive the material
     * for the transaction in this message.
     *
     * @param requester pointer to the new requester
     */
    void setRequester(Model* requester);

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

};
#endif
