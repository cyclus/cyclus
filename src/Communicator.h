// Communicator.h
// An abstract class for deriving simulation entities that can communicate.

#if !defined(_COMMUNICATOR)
#define _COMMUNICATOR
#include <iostream>
#include <string>

#include "Message.h"
#include "Material.h"

using namespace std;


/**
 * The types of Communicators (Logician, Regions, Insts, and Facilities).
 */
enum CommunicatorType {LogicianEnum, RegionEnum, InstEnum, FacilityEnum};

/**
 * An abstract class for deriving simulation entities that can communicate.
 */
class Communicator {

	protected:
	/**
	 * Writes a Message with the given specs.
	 * @param dir the direction this Message is traveling
	 * @param commod the Commodity being offered or requested
	 * @param amount the amount of the given Commodity being offered or 
	 * requested
	 * @param price the price of the Commodity
	 * @param theFac the ID number of the Facility object for which the 
	 * Message is being sent, if applicable (null otherwise).
	 */
	virtual Message* writeMessage(MessageDir dir, 
		Commodity* commod, double amount, double price, int theFac);

	public:

	/**
	 * Destroys this Communicator.
	 */
	virtual ~Communicator();

	/**
	 * Equality operator for Communicators. It's sufficient to test whether 
	 * they have the same ID number and same type and return true iff so.
	 */
	virtual bool equals(Communicator const * rhs) const;

	/**
	 * Returns the name of this Communicator. Actually, this version just 
	 * throws an exception, since the name needs to be returned by the child.
	 *
	 * @return the name of this Communicator
	 */
	virtual const string getName() const;

	/**
	 * Returns the ID number for this Communicator. Derived classes should 
	 * override this function.
	 *
	 * @return this Communicator's ID number.
	 */
	virtual int getSN() const;

	/**
	 * Subset operator for Communicators. 
	 */
	virtual bool isSubsetOf(Communicator const * rhs) const;

	/**
	 * Superset operator for Communicators.
	 */
	virtual bool isSupersetOf(Communicator const * rhs) const;

	/**
	 * Proper subset operator for Communicators. 
	 */
	virtual bool isStrictSubsetOf(Communicator const * rhs) const;

	/**
	 * Proper superset operator for Communicators.
	 */
	virtual bool isStrictSupersetOf(Communicator const * rhs) const;

	/**
	 * Receives and possibly handles a Message.
	 *
	 * @param theMessage the Message being received
	 */
	virtual void receiveMessage(Message* theMessage);

	/**
	 * Executes the order represented by the given Messages.
	 * 
	 * @param needsMat the (possibly altered) request some entity interested
	 * in taking on the Material
	 * @param hasMat the (possibly altered) offer this Facility sent in hopes 
	 * that someone would want its Material
	 * @param time the current time
	 */
	virtual void executeOrder(Message* needsMat, Message* hasMat, int time);

	/**
	 * Placeholder virtual function to be overridden by actors that can 
	 * actually sendMaterial.
	 *
	 * @param mat the Material to be sent
	 * @param rec the receiver of the given Material
	 * @param time the current time
	 */
	virtual void sendMaterial(Material* mat, Communicator* rec, int time);

	/**
	 * Placeholder virtual function to be overridden by actors that can 
	 * actually receiveMaterial.
	 *
	 * @param mat the Material to be received
	 * @param time the current time
	 */
	virtual void receiveMaterial(Material* mat, int time);

	/**
	 * Turns the given string into the corresponding member of the 
	 * CommunicatorType enumeration, if it matches one. Throws a GenException 
	 * otherwise.
	 *
	 * @param s the string representation of the Communicator type
	 *
	 * @return the enumerator of same
	 */
	static CommunicatorType enumerateCommunicatorType(string s);

	/**
	 * Insertion stream operator for a Communicator.
	 */
	friend ostream& operator<<(ostream &os, const Communicator& c);

	/**
	 * Insertion stream operator for a pointer to a Communicator.
	 */
	friend ostream& operator<<(ostream &os, const Communicator* c);

};
#endif
