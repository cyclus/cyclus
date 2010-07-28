// Message.h
// A Message class for inter-entity requests, instructions, etc.

#if !defined(_MESSAGE)
# define _MESSAGE

#include "Material.h"

class Communicator;

/**
 * An enumerative type to specify which direction (up or down the class 
 * hierarchy) this message is moving.
 */
enum MessageDir {up, down};

/**
 * A Message class for inter-entity communication.
 */
class Message {

	private:

	/**
	 * The direction this message is traveling (up or down the class 
	 * hierarchy).
	 */
	MessageDir myDir;
		
	/**
	 * The Commodity that is being requested or offered in this Message.
	 */
	Commodity* myCommod;

	/**
	 * The Communicator who sent this Message.
	 */
	Communicator* sender;

	/**
	 * The Communicator who will receive this Message.
	 */
	Communicator* receiver;

  /**
   * The Communicator who is the supplier in this transaction.
   */
  Communicator* supplier;

  /**
   * The Communicator who is the requester in this transaction.
   */
  Communicator* requester

	/**
	 * The amount of the specified commodity being requested or offered. 
	 * Units vary. 
	 * 
	 * Note: positive amounts mean you want something, negative amounts 
	 * mean you want to get rid of something.
	 */
	double myAmount;

	/**
	 * The minimum amount of the specified commodity being requested or offered. 
	 * Units vary. 
	 * 
	 * Note: positive amounts mean you want something, negative amounts 
	 * mean you want to get rid of something.
	 */
	double minAmount;


	/**
	 * The price of the commodity being requested or offered.
	 */
	double myPrice;

	public:

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
	 * @param toReceive the receiver of this Message, or null if the 
	 * eventual receiver/handler is unknown to the sender
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
	 * Returns the receiver of this Message.
	 *
	 * @return the receiver
	 */
	Communicator* getReceiver() const;

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
	 * Sets the amount of some Commondity being requested or offered in this 
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
	void setSupplier(double newSupplier);

	/**
   * Sets the assigned requester to receive the material
   * for the transaction in this message.
	 *
	 * @param newRequester the updated requester
	 */
	void setRequester(double newRequester);

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
	 * unEnumerates the message direction.
	 * 
	 * @return the string associated with myDir
	 */
	string unEnumerateDir();

};
#endif
