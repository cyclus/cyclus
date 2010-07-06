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
	 * The amount of the specified commodity being requested or offered. 
	 * Units vary. 
	 *
	 * cake -- mass of uranium (metric tons)
	 * uUF6 -- mass of uranium (metric tons)
	 * eUF6 -- number of batches
	 * fuel -- number of batches (reactor), tons of uranium (fabricator)
	 * usedFuel -- number of assemblies (for now, at least)
	 * 
	 * Note: positive amounts mean you want something, negative amounts 
	 * mean you want to get rid of something.
	 */
	double myAmount;

	/**
	 * The price of the commodity being requested or offered.
	 */
	double myPrice;

	/**
	 * The Facility ID for the Facility originally sending this Message, 
	 * if applicable.
	 */
	int facID;

	public:

	/**
	 * Creates a new Message with the given specs.
	 *
	 * @param dir the direction this Message is traveling
	 * @param commod the Commodity being offered or requested
	 * @param amount the amount of the given Commodity being offered or 
	 * requested (pass the number of batches if requesting fuel)
	 * Note: positive amounts mean you want something, negative amounts 
	 * mean you want to get rid of something.
	 * @param price the price of the Commodity
	 * @param toSend the sender of this Message
	 * @param toReceive the receiver of this Message, or null if the 
	 * eventual receiver/handler is unknown to the sender
	 * @param facID the Facility object sending this 
	 * Message, or the Facility on whose behalf this request is being made.
	 */
	Message(MessageDir dir, Commodity* commod, 
			double amount, double price, Communicator* toSend, 
			Communicator* toReceive, int facID);

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
	 * Returns the price being requested or offered in this message.
	 *
	 * @return the price (in dollars)
	 */
	double getPrice() const;

	/**
	 * Returns the pointer to the Facility object included with this message. 
	 * This object may be useful in describing the core we're trying to make fuel for.
	 *
	 * @return the facID of the Facility object
	 */
	int getFacID() const;

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

	/**
	 * Insertion stream operator for a Message.
	 */
	friend ostream& operator<<(ostream &os, const Message& m);

	/**
	 * Insertion stream operator for a pointer to a Message.
	 */
	friend ostream& operator<<(ostream &os, const Message* m);
		
};
#endif
