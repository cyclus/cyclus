// Message.cpp
// Implements the Message class.

#include "Message.h"
#include "Communicator.h"
#include "GenException.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Message::Message(MessageDir dir, Commodity* commod, double amount, double price,
		 Communicator* toSend, Communicator* toReceive) 
{
	myCommod = commod;
	myAmount = amount; 
	myPrice = price;
	myDir = dir;
	facID = ID;
  sender = toSend;
  recipient = toReceive;
  
  // if amt is positive and there is no supplier
  // this message is an offer and 
  // the sender is the supplier
  if (myAmount > 0  && supplier == NULL){
    supplier = sender;
  }

  // if amt is negative and there is no requester
  // this message is a request and
  // the sender is the requester
  if (myAmount < 0 && requester == NULL){
    requester = sender;
  }
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Message* Message::clone() const
{
	return new Message(myDir, myCommod, myAmount, myPrice, sender, receiver,
										 facID);
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Communicator* Message::getSender() const {
	return sender;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Communicator* Message::getReceiver() const {
return receiver;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MessageDir Message::getDir() const {
	return myDir;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Commodity* Message::getCommod() const {
	return myCommod;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double Message::getAmount() const {
	return myAmount;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Message::setAmount(double newAmount) 
{
	myAmount = newAmount;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Message::setSupplier(Communicator* newSupplier) 
{
	supplier = newSupplier;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Message::setRequester(Communicator* newRequester) 
{
	requester = newRequester;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double Message::getPrice() const {
	return myPrice;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int Message::getFacID() const
{
	return facID;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Message::reverseDirection()
{
	if (down == myDir)
		myDir = up; 
	else
		myDir = down;
		// Speaking of "getting up" or "getting down," check out
		// "National Funk Congress Deadlocked On Get Up/Get Down Issue,"
		// The Onion (1999, October 27), 35(39).
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
string Message::unEnumerateDir(){
	string toRet;
	if (up == myDir)
		toRet = "up";
	else if (down == myDir)
		toRet = "down";
	else
		throw GenException("Attempted to send a message neither up nor down.");

	return toRet;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream& operator<<(ostream &os, const Message& m)
{
	string commod = (m.myCommod)->getName();
	os << "Message" << endl 
		 << "... = sender = " << m.sender->getName() << endl
		 << "... = myCommodity = " <<  commod.c_str() << endl
		 << "... = myAmount = " << m.myAmount << endl;

	return os;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream& operator<<(ostream &os, const Message* m)
{
	os << *m;
	return os;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
