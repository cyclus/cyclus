// Communicator.cpp
// Implements the Communicator class.

#include "Communicator.h"
#include "GenException.h"
#include "Region.h"
#include "Inst.h"
#include "Facility.h"
#include "Logician.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Message* Communicator::writeMessage(MessageDir dir, Commodity* commod, double
                                      amount, double price, int theFac)
{
	Communicator* me = this;
	Communicator* receiver = 0;
	return new Message(dir, commod, amount, price, me, receiver, theFac); 
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Communicator::~Communicator()
{

}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool Communicator::equals(Communicator const * rhs) const
{
	// Return false if the ID numbers aren't the same, since if not there's no 
	// chance of lhs == rhs.
	Communicator* lhs = (Communicator*) this;
	int leftID = lhs->getSN();
	int rightID = rhs->getSN();
	if (leftID != rightID)
	 	return false;

	// If we made it this far, lhs == rhs iff they have the same type. Return 
	// true if so, false otherwise.

	if (dynamic_cast<const Region*>(lhs))
		if (dynamic_cast<const Region*>(rhs))
			return true;

	if (dynamic_cast<const Inst*>(lhs))
		if (dynamic_cast<const Inst*>(rhs))
			return true;

	if (dynamic_cast<const Facility*>(lhs))
		if (dynamic_cast<const Facility*>(rhs))
			return true;

	// Otherwise, return false. We know it's not true that 
	// lhs == rhs == the Logician because Logician doesn't override getSN(), so 
	// we would have thrown an exception already.
	return false;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const string Communicator::getName() const
{
	throw GenException("Tried to get name of a Communicator that doesn't overwrite getName()");
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int Communicator::getSN() const
{
	// It's (apparently) possible that this Communicator doesn't know it's 
	// something more than a Communicator (i.e., sometimes overridden versions of 
	// this function don't get called).
	// KDHFLAG ^^^ I don't understand the implications of this comment. 
	// KDHFLAG ... Does the code below fix it? 
	const Communicator* thisp = (Communicator*) this;
	if (dynamic_cast<const Region*>(thisp)) {
		const Region* r = (const Region*) thisp;
		return r->getSN();
	}
	if (dynamic_cast<const Inst*>(thisp)) {
		const Inst* r = (const Inst*) thisp;
		return r->getSN();
	}
	if (dynamic_cast<const Facility*>(thisp)) {
		const Facility* r = (const Facility*) thisp;
		return r->getSN();
	}

	throw GenException("Tried to get ID number of a class that doesn't override getSN()");
	return -1;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool Communicator::isSubsetOf(Communicator const * rhs) const
{
	return (this->equals(rhs)) || (this->isStrictSubsetOf(rhs));
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool Communicator::isSupersetOf(Communicator const * rhs) const
{
	return (this->equals(rhs)) || (this->isStrictSupersetOf(rhs));
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool Communicator::isStrictSubsetOf(Communicator const * rhs) const
{
	// Let's keep the Logician out of things.
	if (dynamic_cast<const Logician*>(this) || dynamic_cast<const Logician*>(rhs))
		throw GenException("Don't use < operator with the Logician.");

	// Now let's walk through all the possibilities. We'll let the default be 
	// false, so we only need to explicitly deal with cases where it's possible
	// that this < rhs is true.

	// ######## cases where this is a Region ########
	// R < R: never possible (a set can be a subset of itself but not a 
	// proper subset of itself--that's the definition of proper subset)
	// R < I: never possible
	// R < F: never possible

	// ######## cases where this is an Inst ########
	// I < R: true iff I is located in R
	// I < I: never possible (by def. of proper subset)
	// I < F: never possible
	if (dynamic_cast<const Inst*>(this) && dynamic_cast<const Region*>(rhs)) {
		Region* r = (Region*) rhs;
		Inst* i = (Inst*) this;		

		// If the Inst isn't in the Region, the try block will throw an exception.
		try {
			r->getInst(i->getSN());
			return true;
		}
		catch(GenException ge){
			return false;
		}
	}

	// ######## cases where this is a Facility ##########
	// F < R: true iff F's owner is in R
	// F < I: true iff I owns F
	// F < F: never possible (by def. of proper subset)

	if (dynamic_cast<const Facility*>(this)) {
		Facility* f = (Facility*) this;

		if (dynamic_cast<const Inst*>(rhs)) {
			Inst* i = (Inst*) rhs;

			// If the Facility isn't in the Inst, the try block will throw.
			try {
				i->getFac(f->getSN());
				return true;
			}
			catch(GenException fe){
				return false;			
			}
		}

		if (dynamic_cast<const Region*>(rhs)) {
			Region* r = (Region*) rhs;

			// If the Facility's owner isn't in the Region, the try block will throw.
			try {
				r->getInst(f->getInst()->getSN()); // hehe, sorry
				return true;
			}
			catch(GenException ge){
				return false;
			}
		}
	}

	return false;

}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool Communicator::isStrictSupersetOf(Communicator const * rhs) const
{
	// Let's keep the Logician out of things.
	if (dynamic_cast<const Logician*>(this) || dynamic_cast<const Logician*>(rhs))
		throw GenException("Don't use >= operator with the Logician.");

	// I'm not going to comment this method very thoroughly, since it's 
	// essentially the converse or inverse or something of the < operator just 
	// above, which is commented in detail.

	// ######## cases where this is a Region ########
	// R > R: never possible (a set can be a subset of itself but not a 
	// proper subset of itself--that's the definition of proper subset)
	// R > I: true iff I is located in R
	// R > F: true iff F's owner is in R
	if (dynamic_cast<const Region*>(this)) {
		Region* r = (Region*) this;

		if (dynamic_cast<const Inst*>(rhs)) {
			Inst* i = (Inst*) rhs;

			try {
				r->getInst(i->getSN());
				return true;
			}
			catch(GenException ge) {
				return false;
			}
		}

		if (dynamic_cast<const Facility*>(rhs)) {
			Facility* f = (Facility*) rhs;

			try {
				r->getInst(f->getInst()->getSN()); // hehe, sorry
				return true;
			}
			catch(GenException ge) {
				return false;
			}
		}
	}

	// ######## cases where this is an Inst ########
	// I > R: never possible
	// I > I: never possible (by def. of proper subset)
	// I > F: true iff I owns F
	if (dynamic_cast<const Inst*>(this) || dynamic_cast<const Facility*>(rhs)) {
		Inst* i = (Inst*) this;
		Facility* f = (Facility*) rhs;

		try {
			i->getFac(f->getSN());
			return true;
		}
		catch(GenException fe) {
			return false;
		}
	}

	// ######## cases where this is a Facility ##########
	// F > R: never possible
	// F > I: never possible
	// F > F: never possible (by def. of proper subset)
	return false;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Communicator::receiveMessage(Message* theMessage) 
{
	throw GenException
		("Class receiving Message didn't override receiveMessage()");
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Communicator::executeOrder(Message* needsMat, Message* hasMat, int time)
{
	throw GenException("Class executing order didn't override executeOrder()");
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Communicator::sendMaterial(Material* mat, Communicator* rec, int time)
{
	throw GenException("Class sending Material didn't override sendMaterial()");
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Communicator::receiveMaterial(Material* mat, int time)
{
	throw GenException
		("Class receiving Material didn't override receiveMaterial()");
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream& operator<<(ostream &os, const Communicator& c)
{
	os << c.getName();
	return os;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream& operator<<(ostream &os, const Communicator* c)
{
	os << c->getName();
	return os;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CommunicatorType Communicator::enumerateCommunicatorType(string s)
{
	if ("Logician" == s)
		return LogicianEnum;
	if ("Region" == s)
		return RegionEnum;
	if ("Inst" == s)
		return InstEnum;
	if ("Facility" == s)
		return FacilityEnum;
	throw GenException("Error: unrecognized Communicator type to enumerate.");
}
